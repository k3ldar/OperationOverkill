#include <SPI.h>             /* to handle the communication interface with the modem*/
#include <nRF24L01.h>        /* to handle this particular modem driver*/
#include <RF24.h>            /* the library which helps us to control the radio modem*/

#include "SerialCommandManager.h"
#include "RFCommunicationManager.h"



char SenderId = 0;
const byte readAddress[5] = {'R','F','0','0','A'};
const byte writeAddress[5] = {'R','F','0','0','B'};



#define RF_CE_PIN 9
#define RF_CSN_PIN 10

RF24 radio(RF_CE_PIN, RF_CSN_PIN);
SerialCommandManager commandMgr(&CommandReceived, '\n', ':', '=', 500, 256);
RFCommunicationManager rfCommandMgr(SenderId, true, &radio);
unsigned long nextSend = 0;

void connectToRadio()
{
  bool isConnected = radio.begin();
  if (!isConnected)
  {
    Serial.println("Radio not responding");
	  return;
  }
  
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
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
  rfCommandMgr.initialize(); 
}

void CommandReceived()
{
  /*
  if (commandMgr.getCommand() == "CHIP")
  {
    commandMgr.sendCommand("CHIP", String(rfCommandMgr.isChipConnected()));
    return;
  }
  
  if (commandMgr.getCommand() == "DRAT")
  {
    commandMgr.sendCommand("DRAT", String(rfCommandMgr.getDataRate()));
    return;
  }

  if (commandMgr.getCommand() == "SDRAT" && commandMgr.getArgCount() == 1)
  {
    StringKeyValue argValue = commandMgr.getArgs(0);
    radio.setDataRate(static_cast<rf24_datarate_e>(argValue.key.toInt()));
  }

  if (commandMgr.getCommand() == "IPV")
  {
    commandMgr.sendCommand("IPV", String(rfCommandMgr.isPVariant()));
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
    String broadcastData = String("0/T1/") + String(argValue.value);
    broadcastRFData(broadcastData);
    return;
  }
  */
  commandMgr.sendDebug("UNKNOWN", commandMgr.getCommand());
}


void loop()
{
    commandMgr.readCommands();

    if (rfCommandMgr.isInitialized())
    {
      rfCommandMgr.process();

    }

    unsigned long currMillis = millis();
    if (currMillis > nextSend)
    {
      RFPacket data;
      data.senderId = 123;
      radio.write(&data, sizeof(RFPacket));
    }
}
