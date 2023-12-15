#ifndef __CommandMessage__
#define __CommandMessage__


#include <stdlib.h>
#include <String.h>

class CommandMessage
{
private:
	String _command;
	StringKeyValue _params[5];
	int _paramCount;
	String _rawMessage;
	bool _messageComplete;
	bool _messageTimeout;
public:
	CommandMessage();
	bool isTimeout();
	bool isCompleteMessage();
	String getCommand();
	StringKeyValue getArgs(int index);
	int getArgCount();
	String getRawMessage();
	void
};

#endif