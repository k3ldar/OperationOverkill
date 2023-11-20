#include <SPI.h>             /* to handle the communication interface with the modem*/
#include <nRF24L01.h>        /* to handle this particular modem driver*/
#include <RF24.h>            /* the library which helps us to control the radio modem*/
#include "SerialCommandManager.h"


#define RF_CE_PIN 9
#define RF_CSN_PIN 10

void CommandReceived();

SerialCommandManager commandMgr(CommandReceived, '\n', ':', '=', 500, 256);

RF24 radio(RF_CE_PIN, RF_CSN_PIN);
const byte RFAddress[6] = "SI001";


void setup()
{
  Serial.begin(115200);
  
  radio.begin();
  radio.openReadingPipe(0, RFAddress);
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
    radio.setDataRate(argValue.key.toInt());
  }

  if (commandMgr.getCommand() == "IPV")
  {
    commandMgr.sendCommand("IPV", String(radio.isPVariant()));
    return;
  }
  
  commandMgr.sendDebug("UNKNOWN", commandMgr.getCommand());
}

void loop()
{
    commandMgr.readCommands();

    if (radio.available())
    {
        char text[32] = "";
        radio.read(&text, sizeof(text));
        String receivedText = String(text);
        
        if (receivedText.length() > 0)
        {
            commandMgr.sendCommand("RF", text);
        }
    }
}
