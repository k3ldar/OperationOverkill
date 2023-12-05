#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Queue.h>
#include <MemoryFree.h>

#define SERIAL_DEBUG
#define Relay_1_Threshold 400
#define Relay_2_Threshold 420
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

const byte RF_NODE_ID = 1;

#define WaterSensor_Signal_Pin A4

bool Relay1Active = false;
byte Relay1Readings = 0;
bool Relay2Active = false;

Queue queue(15);

unsigned long myTime;
int sensor1Value = 0;
unsigned long turnPump1OffMilli = 0;
unsigned long turnPump2OffMilli = 0;

RF24 radio(RF_CE_PIN, RF_CSN_PIN);
const byte RFAddress[6] = "RF001";

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
  
  if (!radio.begin())
    Serial.println("Radio not responding");

  radio.setRetries(6, 4);
  radio.openWritingPipe(RFAddress);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
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
    digitalWrite(Relay_1_LED_PIN, HIGH);
    digitalWrite(Relay_1_PIN, HIGH);
    Serial.println("Relay 1 ON");
    Relay1Active = true;
    turnPump1OffMilli = millis() + MinimumPumpTimeMs;
}

void processPump1(unsigned long currTime, int average)
{
    if (Relay1Active)
    {
        if (currTime >= turnPump1OffMilli || sensor1Value <= MinimumCutoffThreshold)
        {
          turnPump1Off();
        }
    }
    else if (average >= Relay_1_Threshold)
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

void processPump2(unsigned long currTime, int average)
{
    if (Relay2Active)
    {
        if (currTime > turnPump2OffMilli || Relay2Active && !Relay1Active || sensor1Value < MinimumCutoffThreshold || sensor1Value < Relay_2_Threshold)
        {
          turnPump2Off();
        }
    }
    else if (average > Relay_2_Threshold)
    {
        turnPump2On();
    }
}

void loop()
{
  processWaterSensor();

  /*if (radio.available())
  {
        char text[32] = "";
        radio.read(&text, sizeof(text));
        String receivedText = String(text);
        
        if (receivedText.length() > 0)
        {
            Serial.println(text);
        }
  }*/
}

void processWaterSensor()
{
    unsigned long currTime = millis();
    bool validateSensor = false;
    
    if (currTime - myTime > ReadSensorMs)
        validateSensor = true;
  
    if (validateSensor)
    {
        myTime = currTime;
    
        int s1Value = getSensorValue();

        if (queue.isFull())
          queue.dequeue();
          
        queue.enqueue(s1Value);
        
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

        int average = queue.average();

        if (queue.isFull())
        {
          processPump1(currTime, average);
          processPump2(currTime, average);
        }

        String combined = "1/WS/" + String(s1Value) + "/" + 
          String(average) + "/" +
          String(Relay1Readings) + "/" + 
          String(Relay1Active) + "/" +
          String(Relay2Active);

        char data[32] = "";
        combined.toCharArray(data, sizeof(data));

        radio.stopListening();
        if (!radio.write(&data, sizeof(data)))
        {
          Serial.println("Failed to write data");
        }
        radio.startListening();
        Serial.println(data);
    }
}
