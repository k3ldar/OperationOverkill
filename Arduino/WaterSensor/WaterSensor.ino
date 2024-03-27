#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "SerialCommandManager.h"
#include "RFCommunicationManager.h"
//#include "WaterPump.h"

char SenderId = '2';
const byte readAddress[6] = {'R','F','0','0','B' };
const byte writeAddress[6] = {'R','F','0','0','A' };

const byte DEVICE_RF_Id = 69;

// test box com 11

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


//WaterPump waterPump(WaterSensor_Signal_Pin, WaterSensor_Power_Pin, Sensor_Active_LED_PIN, Relay_1_LED_PIN, Relay_2_LED_PIN, Relay_1_PIN, Relay_2_PIN);
RF24 radio(RF_CE_PIN, RF_CSN_PIN);
SerialCommandManager commandMgr(&CommandReceived, '\n', ':', '=', 500, 256);
RFCommunicationManager rfCommandMgr(&SendMessage, SenderId, false, &radio);


void SendMessage(String message, MessageType messageType)
{
  switch (messageType)
  {
    case Debug:
      commandMgr.sendDebug(message, "WPd");
      return;
    case Error:
      commandMgr.sendError(message, "WPe");
      return;
    default:
      commandMgr.sendCommand("WPo", message);
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
  
  rfCommandMgr.connectToRadio(readAddress, writeAddress);
  //weatherStation.initialize(&SendMessage, &rfCommandMgr);


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

  if (message.substring(2, 4) == "WP")
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
  //weatherStation.process();

  if (rfCommandMgr.canReconnect())
  {
    rfCommandMgr.connectToRadio(readAddress, writeAddress);
  }

  rfCommandMgr.process();
}

