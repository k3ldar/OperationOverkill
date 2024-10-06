#include <ArduinoJson.h>
#include <AccelStepper.h>

#include "Common.h"
#include "WebClientManager.h"
#include "arduino_secrets.h" 
#include "SerialCommandManager.h"

#include "LedManager.h"

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

#define ABSOLUTE_CCW -10000
#define ABSOLUTE_CW 10000
#define MOVE_CW 1
#define MOVE_CCW -1
#define SPEED_STOPPED 0.0
#define MAX_SPEED 300
#define SPEED 20
#define MINIMUM_SENSOR_DIFF 2

#if defined(RELEASE)
char server[] = "192.168.8.200";
uint16_t port = 80;
#endif

#if defined(DEBUG)
char server[] = "192.168.8.201";
uint16_t port = 7100;gg
#endif

const int LightSensorLeft = A0;
const int LightSensorRight = A1;
const int StepperStep = D2;
const int StepperDirection = D3;
const int LimitSwitchLeft = D6;
const int LimitSwitchRight = D7;

// calibration offsets
const int OffsetSensorLeft = 2;
const int OffsetSensorRight = 0;

long currentMotorPosition = 0;
long nextLightCheck;
bool alignToCenter = true;
long MaximumRotation = 0;
long MinimumRotation = 20;
bool resetForced = false;

AccelStepper myStepper(AccelStepper::DRIVER, StepperStep, StepperDirection);


void setup() {
    Serial.begin(115200);
    while (!Serial);

    // stepper motor
    myStepper.setMaxSpeed(MAX_SPEED);
    myStepper.setSpeed(SPEED_STOPPED);
    myStepper.setAcceleration(1);
    myStepper.disableOutputs();
    myStepper.setCurrentPosition(0);

    // limit switches
    pinMode(LimitSwitchLeft, INPUT_PULLUP);
    pinMode(LimitSwitchRight, INPUT_PULLUP);

    // general
    nextLightCheck = millis() + 1000;
}

void loop()
{
    // if both limit switches are pressed then reset
    if (IsLeftLimitReached() && IsRightLimitReached())
        resetForced = true;

    if (resetForced && !IsLeftLimitReached() && !IsRightLimitReached())
    {
        resetForced = false;
        alignToCenter = true;
    }

    long currMillis = millis();


    if (alignToCenter)
    {
        RecenterStand();
    }
    else
    {
        CheckLightPosition(currMillis);
    }
}

void RecenterStand()
{
    myStepper.setCurrentPosition(0);
    myStepper.enableOutputs();

    MoveAbsoluteCCW();
    MoveAbsoluteCW();

    Serial.print("Minimum: ");
    Serial.println(MinimumRotation);

    Serial.print("Maximum: ");
    Serial.println(MaximumRotation);

    MoveToCenter();
    
    alignToCenter = false;
    myStepper.disableOutputs();
}

bool IsLeftLimitReached()
{
    return digitalRead(LimitSwitchLeft) == 0;
}

bool IsRightLimitReached()
{
    return digitalRead(LimitSwitchRight) == 0;
}

void StepperRun()
{
    myStepper.setSpeed(SPEED);
    myStepper.runSpeedToPosition();

    Serial.print("Current Stepper Position: ");
    Serial.println(myStepper.currentPosition());
}

void StepperStop()
{
    myStepper.setSpeed(SPEED_STOPPED);
    myStepper.stop();
}

void MoveAbsoluteCCW()
{
    myStepper.moveTo(ABSOLUTE_CCW);
    do
    {
        bool limitReached = IsLeftLimitReached();
        Serial.print("Left limit reached: ");
        Serial.println(limitReached);

        if (limitReached)
        {
            Serial.println("Left Limit Reached, stopping");
            StepperStop();

            // pull back until limit switch disengages
            do
            {
                limitReached = IsLeftLimitReached();
                myStepper.move(MOVE_CW);
                StepperRun();
            } while (limitReached);

            StepperStop();
            
            myStepper.setCurrentPosition(0);

            // set home position
            MinimumRotation = myStepper.currentPosition();
            return;
        }
        else
        {
            StepperRun();
        }
    } while (true);
}

void MoveAbsoluteCW()
{
    myStepper.moveTo(ABSOLUTE_CW);
    do
    {
        bool limitReached = IsRightLimitReached();
        Serial.print("Right limit reached: ");
        Serial.println(limitReached);

        if (limitReached)
        {
            Serial.println("Right Limit Reached, stopping");
            StepperStop();

            // pull back until limit switch disengages
            do
            {
                limitReached = IsRightLimitReached();
                myStepper.move(MOVE_CCW);
                StepperRun();
            } while (limitReached);

            StepperStop();
            // set maximum range position
            MaximumRotation = myStepper.currentPosition();
            return;
        }
        else
        {
            StepperRun();
        }
    } while (true);
}

void MoveToCenter()
{
    long halfWay = (MaximumRotation - MinimumRotation) / 2;
    Serial.print("Half way: ");
    Serial.println(halfWay);

    while (myStepper.currentPosition() > halfWay)
    {
        myStepper.move(MOVE_CCW);
        StepperRun();
    }

    Serial.print("Stepper Position: ");
    Serial.println(myStepper.currentPosition());
    StepperStop();
}

void CheckLightPosition(long currMillis)
{
    if (currMillis > nextLightCheck)
    {
        nextLightCheck = currMillis + 1000;

        int rawLightLeft = analogRead(LightSensorLeft);
        int lightLeft = map(rawLightLeft, 0, 1023, 0, 100) + OffsetSensorLeft;

        int rawLightRight = analogRead(LightSensorRight);
        int lightRight = map(rawLightRight, 0, 1023, 0, 100) + OffsetSensorRight;

        Serial.print("Light level Left: "); 
        Serial.print(rawLightLeft);
        Serial.print("/");
        Serial.println(lightLeft); // print the light value in Serial Monitor

        Serial.print("Light level Right: "); 
        Serial.print(rawLightRight);
        Serial.print("/");
        Serial.println(lightRight); // print the light value in Serial Monitor

        int fromLeft = lightLeft - lightRight;
        Serial.print("From Left: ");
        Serial.println(fromLeft);

        int diff = lightRight - lightLeft;
        Serial.print("diff: ");
        Serial.println(diff);


        if (diff < -MINIMUM_SENSOR_DIFF || diff  > MINIMUM_SENSOR_DIFF)
        {
            myStepper.enableOutputs();
            if (diff > 0)
            {                
                if (myStepper.currentPosition() -1 < MinimumRotation)
                {
                    // max position reached, can't move any more
                    Serial.println("Minimum rotation reached");
                }
                else
                {
                    Serial.println("Moving CCW");
                    myStepper.move(MOVE_CCW);
                    StepperRun();
                }
            }
            else if (diff < 0)
            {
                if (myStepper.currentPosition() + 1 > MaximumRotation)
                {
                    // max position reached, can't move any more
                    Serial.println("Maximum rotation reached");
                }
                else
                {
                    Serial.println("Moving CW");
                    myStepper.move(MOVE_CW);
                    StepperRun();
                }
            }

            myStepper.disableOutputs();
            Serial.print("Min: ");
            Serial.print(MinimumRotation);
            Serial.print("; ");
            Serial.print("Max: ");
            Serial.print(MaximumRotation);
            Serial.print("; ");
            Serial.print("Curr: ");
            Serial.println(myStepper.currentPosition());
        }
    }
}
