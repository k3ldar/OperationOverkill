#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "SerialCommandManager.h"
#include "RFCommunicationManager.h"
#include "WeatherStation.h"

char SenderId = '1';
const byte readAddress[5] = {'R','F','0','0','B'};
const byte writeAddress[5] = {'R','F','0','0','A'};


const byte DEVICE_RF_Id = 68;
#define RF_CE_PIN 9
#define RF_CSN_PIN 10

#define TEMP_SENSOR_PIN 4
#define RAIN_SENSOR_ANALOG_PIN A0

RF24 radio(RF_CE_PIN, RF_CSN_PIN);
SerialCommandManager commandMgr(&CommandReceived, '\n', ':', '=', 500, 256);
RFCommunicationManager rfCommandMgr(SenderId, false, &radio);
WeatherStation weatherStation(TEMP_SENSOR_PIN, RAIN_SENSOR_ANALOG_PIN);

void SendMessage(String message, MessageType messageType)
{
  switch (messageType)
  {
    case Debug:
      commandMgr.sendDebug(message, "WS");
      return;
    case Error:
      commandMgr.sendError(message, "WS");
      return;
    default:
      commandMgr.sendCommand("WS", message);
      return;
  }
}

void CommandReceived()
{
}

void connectToRadio()
{
  bool isConnected = radio.begin();
  if (!isConnected)
  {
    Serial.println("Radio not responding");
	  return;
  }
  
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setRetries(5, 5);

  radio.openWritingPipe(writeAddress);

  radio.openReadingPipe(1, readAddress);

  radio.startListening();
  
  Serial.print("Data rate: ");
  Serial.println(radio.getDataRate());
  Serial.print("Ip Variant: ");
  Serial.println(radio.isPVariant());
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  connectToRadio();

  rfCommandMgr.initialize(&SendMessage);
  weatherStation.initialize(&SendMessage, &rfCommandMgr);
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

void loop()
{
  commandMgr.readCommands();
  weatherStation.process();
  rfCommandMgr.process();
}


