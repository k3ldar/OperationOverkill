#ifndef __WaterPump__H
#define __WaterPump__H

#include <Arduino.h>
#include "Queue.h"

#include "RFCommunicationManager.h"
#include "Common.h"

#define ReadSensorMs 1000
#define Relay_1_Threshold 400
#define Relay_2_Threshold 420
#define MinimumCutoffThreshold 370
#define MinimumPumpTimeMs 4000
#define MinimumWorkingTemperature 1.0
#define TemperatureNotSet -99.9
#define TemperatureInitialValue -101

const unsigned long ValidTemperatureLength = 3600000; // 1 hour in ms


class WaterPump
{
private:
	Queue *_queue;
	SendMessageCallback *_sendMessageCallback;
	int _sensorSignalPin;
	int _sensorValue;
	int _sensorActivePin;
	int _sensorActiveLEDPin;
	int _pump1LEDPin;
	int _pump2LEDPin;
	int _pump1Pin;
	int _pump2Pin;
	bool _pump1Active;
	bool _pump2Active;
	double _currentTemperature;
	unsigned long _nextTemperatureCheck;
	
	unsigned long _myTime;
	unsigned long _turnPump1OffMilli = 0;
	unsigned long _turnPump2OffMilli = 0;
	
	void processPump1(unsigned long currTime, int average);
	void turnPump1On();
	void turnPump1Off();
	void processPump2(unsigned long currTime, int average);
	void turnPump2On();
	void turnPump2Off();
	
	int getSensorValue();
public:
    WaterPump(const int sensorSignalPin,
			  const int sensorActivePin,
			  const int sensorActiveLEDPin,
			  const int pump1LEDPin,
			  const int pump2LEDPin,
			  const int pump1Pin,
			  const int pump2Pin);
	~WaterPump();
	
	void initialize(SendMessageCallback *_sendMessageCallback);
	void process();
	bool pump1Active();
	bool pump2Active();
	double temperatureGet();
	void temperatureSet(double temperature);
};

#endif
