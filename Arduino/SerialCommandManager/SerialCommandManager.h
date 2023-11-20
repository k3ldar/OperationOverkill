#ifndef __SerialCommandManager__
#define __SerialCommandManager__


#include <stdlib.h>
#include <Arduino.h>
#include <String.h>
#include <MemoryFree.h>
#include <Battery.h>


#if (defined(ARDUINO) && ARDUINO >= 155) || defined(ESP8266)
 #define YIELD yield();
#else
 #define YIELD
#endif

typedef struct StringKeyValue {
   String key;
   String value;
} keyAndValue;

void commandReceived();

class SerialCommandManager
{
private:
	String _command;
	StringKeyValue _params[5];
	int _paramCount;
	String _rawMessage;
	long _serialTimeout;
	int _maximumMessageSize;
	bool _messageComplete;
	bool _messageTimeout;
	char _terminator;
	char _commandSeperator;
	char _paramSeperator;
	bool _isDebug;
	void (*_messageReceived)();
	bool processMessage();
	bool processBatteryMessage();
	void sendMessage(String messageType, String message, String identifier);
public:
	SerialCommandManager(void (*commandReceived)(), char terminator, char commandSeperator, char paramSeperator, long timeoutMilliseconds, int maxMessageSize);
	void readCommands();
	bool isTimeout();
	bool isCompleteMessage();
	String getCommand();
	StringKeyValue getArgs(int index);
	int getArgCount();
	String getRawMessage();
	void sendCommand(String header, String message, String identifier = "", StringKeyValue params[] = {}, int argLength = 0);
	void sendDebug(String message, String identifier);
	void sendError(String message, String identifier);
};

#endif