#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "MemoryFree.h"
#include "WaterPump.h"


#define MAX_PACKET_SIZE 32
const byte RFAddresses[][6] = { 
                                "RF001", // Main receiver (this) (read)
                                "RF002", // Water sensor (this) (write)
                                "RF003"  // Weather station (not used here)
                              };

const byte DEVICE_RF_Id = 49;


// water sensor pins
#define WaterSensor_Signal_Pin A4
#define Sensor_Active_LED_PIN 2
#define Relay_1_LED_PIN 3
#define Relay_2_LED_PIN 4
#define Relay_1_PIN 5
#define Relay_2_PIN 6
#define WaterSensor_Power_Pin 7

#define RF_CE_PIN 8
#define RF_CSN_PIN 9
#define TEMP_UPDATE_MAX 1000 * 60 * 5


WaterPump waterPump(WaterSensor_Signal_Pin, WaterSensor_Power_Pin, Sensor_Active_LED_PIN, Relay_1_LED_PIN, Relay_2_LED_PIN, Relay_1_PIN, Relay_2_PIN);
RF24 radio(RF_CE_PIN, RF_CSN_PIN);

void setup() 
{
  Serial.begin(115200);

  waterPump.initialize(&WriteDataToRF);
  
  if (!radio.begin())
    Serial.println("Radio not responding");

  radio.setRetries(6, 4);
  radio.openWritingPipe(RFAddresses[1]);
  radio.openReadingPipe(1, RFAddresses[0]);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.startListening();
}

void loop()
{
  waterPump.process();

  if (radio.available())
  {
    char text[MAX_PACKET_SIZE] = "";
    radio.read(&text, MAX_PACKET_SIZE);
    String receivedText = String(text);
    
    if (receivedText.length() > 0)
    {
      if (receivedText[0] != DEVICE_RF_Id)
      {          
        ProcessIncomingMessage(receivedText);
      }
    }
  }
}

void ProcessIncomingMessage(String message)
{
  Serial.println(message);
  
  if (message.length() < 5)
  {
    Serial.print("Invalid Length: ");
    Serial.println(message.length());
    return;
  }

  if (message.substring(2, 4) != "T1")
  {
    Serial.print("Invalid Command");
    Serial.println(message.substring(3, 4));
    return;
  }

  double newTemperature = message.substring(5).toDouble();
  waterPump.temperatureSet(newTemperature);
  Serial.print("Setting new temperature to: ");
  Serial.println(newTemperature);
}

void WriteDataToRF(String dataToSend)
{
    char data[MAX_PACKET_SIZE] = "";
    dataToSend.toCharArray(data, MAX_PACKET_SIZE);

    radio.stopListening();
    if (!radio.write(&data, MAX_PACKET_SIZE))
    {
      Serial.println("Failed to write data");
    }

    delay(10);
    radio.startListening();
}
