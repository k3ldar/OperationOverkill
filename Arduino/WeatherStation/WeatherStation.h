#ifndef __WeatherStation__
#define __WeatherStation__

#include <Arduino.h>
#include <Temperature_LM75_Derived.h>
#include "RFCommunicationManager.h"

#define MillisBetweenSensorReading 1000;

class WeatherStation
{
private:
	RFCommunicationManager *_rfCommandMgr;
  Generic_LM75 *_lm75;
	int _tempSensorSignalPin;
	double _currentTemperature;
	float _tempCelsius;

  int _rainSensorAnalogPin;

	unsigned long _nextUpdateTime;

  void readTemperatureSensor();
  void readRainSensor();
public:
	WeatherStation(int tempSensorSignalPin, int rainSensorAnalogPin);
	~WeatherStation();
	void initialize(RFCommunicationManager *rfCommandMgr);
	void process();
};


#endif