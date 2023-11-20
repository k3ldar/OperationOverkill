#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define Relay_1_Threshold 400
#define Relay_2_Threshold 420
#define MinReadingsAboveThreshold 3
#define MinimumCutoffThreshold 370
#define MinimumPumpTimeMs 4000
#define ReadSensorMs 1000

#define Sensor_Active_LED_PIN 2
#define Relay_1_LED_PIN 3
#define Relay_2_LED_PIN 4
#define Relay_1_PIN 5
#define Relay_2_PIN 6
#define WaterSensor_Power_Pin 7
#define RF_CE_PIN 8
#define RF_CSN_PIN 9

#define WaterSensor_Signal_Pin A4

bool Relay1Active = false;
byte Relay1Readings = 0;
bool Relay2Active = false;


unsigned long myTime;
int sensor1Value = 0;
unsigned long turnPump1OffMilli = 0;
unsigned long turnPump2OffMilli = 0;

RF24 radio(RF_CE_PIN, RF_CSN_PIN);
const byte RFAddress[6] = "SI001";

void setup() 
{
  Serial.begin(115200);
  
  pinMode(Sensor_Active_LED_PIN, OUTPUT);
  pinMode(Relay_1_LED_PIN, OUTPUT);
  pinMode(Relay_2_LED_PIN, OUTPUT);
  pinMode(Relay_1_PIN, OUTPUT);
  pinMode(Relay_2_PIN, OUTPUT);
  pinMode(WaterSensor_Power_Pin, OUTPUT);
  
  digitalWrite(Sensor_Active_LED_PIN, LOW);
  digitalWrite(Relay_1_LED_PIN, LOW);
  digitalWrite(Relay_2_LED_PIN, LOW);
  digitalWrite(Relay_1_PIN, LOW);
  digitalWrite(Relay_2_PIN, LOW);
  digitalWrite(WaterSensor_Power_Pin, LOW);

  radio.begin();
  radio.openWritingPipe(RFAddress);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening();
}

int getSensorValue()
{
    digitalWrite(Sensor_Active_LED_PIN, HIGH);
    digitalWrite(WaterSensor_Power_Pin, HIGH);
    delay(10);
    int s1Value = analogRead(WaterSensor_Signal_Pin);
    digitalWrite(WaterSensor_Power_Pin, LOW);
    digitalWrite(Sensor_Active_LED_PIN, LOW);

    return s1Value;
}

void turnPump1Off()
{
    if (!Relay1Active)
        return;
        
    digitalWrite(Relay_1_LED_PIN, LOW);
    digitalWrite(Relay_1_PIN, LOW);
    Serial.println("Relay 1 OFF");
    Relay1Active = false;
}

void turnPump1On()
{
    if (Relay1Readings < MinReadingsAboveThreshold)
    {
        Relay1Readings++;
        Serial.print("Relay 1 Readings below threshold ");
        Serial.println(Relay1Readings);
        return;
    }
      
    digitalWrite(Relay_1_LED_PIN, HIGH);
    digitalWrite(Relay_1_PIN, HIGH);
    Serial.println("Relay 1 ON");
    Relay1Active = true;
    turnPump1OffMilli = millis() + MinimumPumpTimeMs;
}

void processPump1(unsigned long currTime)
{
    if (Relay1Active)
    {
        if (currTime >= turnPump1OffMilli || sensor1Value <= MinimumCutoffThreshold)
        {
          turnPump1Off();
        }
    }
    else if (sensor1Value >= Relay_1_Threshold)
    {
        turnPump1On();
    }
}

void turnPump2Off()
{
    digitalWrite(Relay_2_LED_PIN, LOW);
    digitalWrite(Relay_2_PIN, LOW);
    Serial.println("Relay 2 OFF");
    Relay2Active = false;
}

void turnPump2On()
{
    if (!Relay1Active)
      return;
      
    digitalWrite(Relay_2_LED_PIN, HIGH);
    digitalWrite(Relay_2_PIN, HIGH);
    Serial.println("Relay 2 ON");
    Relay2Active = true;
    turnPump2OffMilli = millis() + MinimumPumpTimeMs;
}

void processPump2(unsigned long currTime)
{
    if (Relay2Active)
    {
        if (currTime > turnPump2OffMilli || Relay2Active && !Relay1Active || sensor1Value < MinimumCutoffThreshold || sensor1Value < Relay_2_Threshold)
        {
          turnPump2Off();
        }
    }
    else if (sensor1Value > Relay_2_Threshold)
    {
        turnPump2On();
    }
}

void loop()
{
    unsigned long currTime = millis();
    bool validateSensor = false;
    
    if (currTime - myTime > ReadSensorMs)
        validateSensor = true;
  
    if (validateSensor)
    {
        myTime = currTime;
    
        int s1Value = getSensorValue();
    
        if (s1Value != sensor1Value)
        {
            if (s1Value < sensor1Value)
            {
                if (Relay1Readings > 0)
                {
                  Relay1Readings--;
                  Serial.print("Relay 1 Readings reset to ");
                  Serial.println(Relay1Readings);
                }
            }
    
            sensor1Value = s1Value;
        }

        String combined = "WS/" + String(s1Value) + "/" + 
          String(Relay1Readings) + "/" + 
          String(Relay1Active) + "/" +
          String(Relay2Active);
          
        char data[32] = "";
        combined.toCharArray(data, sizeof(data));
        radio.write(&data, sizeof(data));
        Serial.println(data);

        processPump1(currTime);
        processPump2(currTime);
    }
}
