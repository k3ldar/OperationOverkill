
#include "SerialCommandManager.h"



//example to get memory
// MEM;
// DEBUG; -- returns the debug mode status
// DEBUG:ON; -- turns debug mode on
// DEBUG:OFF; -- turns debug mode off

SerialCommandManager::SerialCommandManager(MessageReceivedCallback *commandReceived, char terminator, char commandSeperator, char paramSeperator, unsigned long timeoutMilliseconds, unsigned int maxMessageSize)
{
	_messageReceivedCallback = commandReceived;
	_terminator = terminator;
	_commandSeperator = commandSeperator;
	_paramSeperator = paramSeperator;
	_serialTimeout = timeoutMilliseconds;
	_maximumMessageSize = maxMessageSize;
	_isDebug = false;
}

bool SerialCommandManager::isCompleteMessage()
{
	return _messageComplete;
}

bool SerialCommandManager::isTimeout()
{
	return _messageTimeout;
}

String SerialCommandManager::getCommand()
{
	return _command;
}

StringKeyValue SerialCommandManager::getArgs(int index)
{
	return _params[index];
}

int SerialCommandManager::getArgCount()
{
	return _paramCount;
}

String SerialCommandManager::getRawMessage()
{
	return _rawMessage;
}

void SerialCommandManager::readCommands()
{
	if (Serial.available() > 0)
	{
		String incomingMessage;
		incomingMessage.reserve(_maximumMessageSize);
		_messageTimeout = false;
		_messageComplete = false;
		_rawMessage = "";
		long currentMillis = millis();
		bool isCommand = true;
		bool isParamName = true;
		_paramCount = 0;

		while (!_messageComplete) 
		{
			if (Serial.available() > 0)
			{
				char inChar = (char)Serial.read();
				_rawMessage += inChar;

				if (inChar == _terminator)
				{
					_messageComplete = true;
				}
				else if (inChar == _commandSeperator)
				{
					isCommand = false;
					_paramCount++;
					isParamName = true;
					_params[_paramCount -1].key = "";
					_params[_paramCount -1].value = "";
				}
				else if (inChar == _paramSeperator)
				{
					isParamName = false;
				}
				else
				{
					if (isCommand)
					{
						incomingMessage += inChar;
					}
					else
					{
						if (isParamName)
						{
							_params[_paramCount -1].key += inChar;
						}
						else
						{
							_params[_paramCount -1].value += inChar;
						}
					}					
				}

				if (inChar == _terminator)
				{
					_messageComplete = true;
					break;
				}

				if (incomingMessage.length() > _maximumMessageSize)
				{
					Serial.println("ERR:Too Long");
					return;
				}
			}

			if (millis() - currentMillis > _serialTimeout)
			{
				Serial.println("ERR:Timeout");
				_messageTimeout = true;
				return;
			}	  
		}

		int sepChar = incomingMessage.indexOf(_commandSeperator);
		
		if (sepChar > -1)
		{
			_command = incomingMessage.substring(0, sepChar);
		}
		else
		{
			_command = incomingMessage;
		}

		if (!processMessage())
			_messageReceivedCallback();
	}

	YIELD;
}

void SerialCommandManager::sendCommand(String header, String message, String identifier, StringKeyValue params[], int argLength)
{
	if (header.length() == 0)
		return;
	
	Serial.print(header);
	Serial.print(_commandSeperator);
	
	if (message.length() > 0)
	{
		Serial.print(message);
		
		if (argLength > 0)
			Serial.print(_commandSeperator);
	}
	
	
	for (int i = 0; i < argLength; i++)
	{
		Serial.print(params[i].key);
		Serial.print(_paramSeperator);
		Serial.print(params[i].value);
		
		if ((argLength -1) != i)
			Serial.print(_commandSeperator);
	}
	
	if (identifier != "")
		Serial.print(": (" + identifier + ")");

	Serial.println(_terminator);
}

bool SerialCommandManager::processMessage()
{
	sendDebug(_rawMessage, "SerialComdMgr");
		
		
	if (_command == "MEM")
	{
		sendCommand("MEM", String(freeMemory()));
		return true;
	}
	else if (_command == "DEBUG")
	{
		
		if (_paramCount == 1)
		{
			_isDebug = _params[0].key == "ON";
		}
		
		sendCommand("DEBUG", _isDebug ? "ON" : "OFF");
		return true;
	}

	return false;
}

void SerialCommandManager::sendDebug(String message, String identifier)
{
	sendMessage("DEBUG", message, identifier);
}

void SerialCommandManager::sendMessage(String messageType, String message, String identifier)
{
	if (message.length() == 0 || !_isDebug)
		return;
	
	Serial.print(messageType);
	Serial.print(":");
	Serial.print(message);
	
	Serial.print(": (" + identifier + ")");
	
	if (message.endsWith(String(_terminator)))
		Serial.println("");
	else
		Serial.println(_terminator);
}

void SerialCommandManager::sendError(String message, String identifier)
{
	sendMessage("ERR", message, identifier);
}
