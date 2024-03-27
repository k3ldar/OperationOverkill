#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "SerialCommandManager.h"
#include "RFCommunicationManager.h"
#include "WeatherStation.h"

// test box com 6

const byte DEVICE_RF_Id = 68;
#define RF_CE_PIN 8
#define RF_CSN_PIN 9

#define TEMP_SENSOR_PIN 7
#define RAIN_SENSOR_ANALOG_PIN A0



char SenderId = '1';
const byte readAddress[6] = {'R','F','0','0','B'};
const byte writeAddress[6] = {'R','F','0','0','A'};

// test box com 6


bool isStarting = true;
#define TEMP_SENSOR_PIN 7
#define RAIN_SENSOR_ANALOG_PIN A0

RF24 radio(RF_CE_PIN, RF_CSN_PIN);
SerialCommandManager commandMgr(&CommandReceived, '\n', ':', '=', 500, 256);
RFCommunicationManager rfCommandMgr(&SendMessage, SenderId, false, &radio);
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


void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("setup called");
  weatherStation.initialize(&SendMessage, &rfCommandMgr);
  rfCommandMgr.connectToRadio(readAddress, writeAddress);
  isStarting = false;
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
  if (rfCommandMgr.canReconnect())
  {
    rfCommandMgr.connectToRadio(readAddress, writeAddress);
  }

  commandMgr.readCommands();
  weatherStation.process();
  rfCommandMgr.process();
}


