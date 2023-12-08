#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "WeatherStation.h"

#define MAX_PACKET_SIZE 32
const byte RFAddresses[][6] = { 
                                "RF001", // Main receiver (this) (read)
                                "RF002", // Water sensor (not used here)
                                "RF003"  // Weather station (this) (write)
                              };

const byte DEVICE_RF_Id = 50;
#define RF_CE_PIN 9
#define RF_CSN_PIN 10

#define TEMP_SENSOR_PIN 8

WeatherStation weatherStation(TEMP_SENSOR_PIN);
RF24 radio(RF_CE_PIN, RF_CSN_PIN);

void setup()
{
  Serial.begin(115200);
  
  weatherStation.initialize(&WriteDataToRF);

  if (!radio.begin())
    Serial.println("Radio not responding");

  radio.setRetries(6, 4);
  radio.openWritingPipe(RFAddresses[2]);
  radio.openReadingPipe(1, RFAddresses[0]);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.startListening();
}


void ProcessIncomingMessage(String message)
{
  Serial.print("Incoming RF: ");
  Serial.println(message);
  
  if (message.length() < 5)
  {
    Serial.print("Invalid Length: ");
    Serial.println(message.length());
    return;
  }

  if (message.substring(2, 4) == "WS")
  {
    return;
  }
  
  if (message.substring(2, 4) != "T1")
  {
    Serial.print("Invalid Command");
    Serial.println(message.substring(2, 4));
    return;
  }

  double newTemperature = message.substring(5).toDouble();
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

void loop()
{
  weatherStation.process();

  if (radio.available())
  {
    char text[MAX_PACKET_SIZE] = "";
    radio.read(&text, MAX_PACKET_SIZE);
    String receivedText = String(text);
    
    if (receivedText.length() > 0)
    {
      if (receivedText[0] != DEVICE_RF_Id)
      {
        Serial.print("Ignoring: ");
        Serial.println(receivedText);
      }
      else
      {          
        ProcessIncomingMessage(receivedText);
      }
    }
  }
}
