#include <SPI.h>             /* to handle the communication interface with the modem*/
#include <nRF24L01.h>        /* to handle this particular modem driver*/
#include <RF24.h>            /* the library which helps us to control the radio modem*/
#include "SerialCommandManager.h"


#define MAX_PACKET_SIZE 32
const byte RFAddresses[][6] = { 
                                "RF001", // Main receiver (this) (write)
                                "RF002", // Water sensor (read)
                                "RF003"  // Weather station (read)
                              };


#define RF_CE_PIN 9
#define RF_CSN_PIN 10


SerialCommandManager commandMgr(&CommandReceived, '\n', ':', '=', 500, 256);

RF24 radio(RF_CE_PIN, RF_CSN_PIN);


void setup()
{
  Serial.begin(115200);
  
  if (!radio.begin())
    Serial.println("Radio not responding");

  radio.openWritingPipe(RFAddresses[0]);
  radio.openReadingPipe(1, RFAddresses[1]);
  radio.openReadingPipe(2, RFAddresses[2]);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.startListening();
  
  Serial.print("Data rate: ");
  Serial.println(radio.getDataRate());
  Serial.print("Ip Variant: ");
  Serial.println(radio.isPVariant());
}

void CommandReceived()
{
  if (commandMgr.getCommand() == "CHIP")
  {
    commandMgr.sendCommand("CHIP", String(radio.isChipConnected()));
    return;
  }
  
  if (commandMgr.getCommand() == "DRAT")
  {
    commandMgr.sendCommand("DRAT", String(radio.getDataRate()));
    return;
  }

  if (commandMgr.getCommand() == "SDRAT" && commandMgr.getArgCount() == 1)
  {
    StringKeyValue argValue = commandMgr.getArgs(0);
    radio.setDataRate(static_cast<rf24_datarate_e>(argValue.key.toInt()));
  }

  if (commandMgr.getCommand() == "IPV")
  {
    commandMgr.sendCommand("IPV", String(radio.isPVariant()));
    return;
  }

  if (commandMgr.getCommand() == "TCAR")
  {
    commandMgr.sendCommand("TCAR", String(radio.testCarrier()));
    return;
  }

  if (commandMgr.getCommand() == "TRPD")
  {
    commandMgr.sendCommand("TRPD", String(radio.testRPD()));
    return;
  }

  if (commandMgr.getCommand() == "T1")
  {
    StringKeyValue argValue = commandMgr.getArgs(0);
    String broadcastData = String("0/T1/") + String(argValue.key);
    BroadcastRFData(broadcastData);
    return;
  }
  
  commandMgr.sendDebug("UNKNOWN", commandMgr.getCommand());
}

void BroadcastRFData(String data)
{
  char rfData[MAX_PACKET_SIZE] = "";
  data.toCharArray(rfData, MAX_PACKET_SIZE);

  radio.stopListening();

  if (!radio.write(&rfData, MAX_PACKET_SIZE, true))
  {
    Serial.print("Failed to broadcast data: ");
    Serial.println(data);
  }
  
  delay(5);
  
  radio.startListening();
}

void loop()
{
    commandMgr.readCommands();

    if (radio.available())
    {
        char text[MAX_PACKET_SIZE] = "";
        radio.read(&text, MAX_PACKET_SIZE);
        String receivedText = String(text);
        
        if (receivedText.length() > 0)
        {
          BroadcastRFData(text);
          commandMgr.sendCommand("RF", text);
        }
    }
}
