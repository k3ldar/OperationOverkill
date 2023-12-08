#ifndef __WeatherStation__
#define __WeatherStation__

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define MillisBetweenSensorReading 1000;

typedef void RFCallback(String data);

class WeatherStation
{
private:
	RFCallback *_rfCallback;
	OneWire *_oneWire;
	DallasTemperature *_tempSensor;
	int _tempSensorSignalPin;
	double _currentTemperature;
	float _tempCelsius;

	unsigned long _nextUpdateTime;
public:
	WeatherStation(int tempSensorSignalPin);
	~WeatherStation();
	void initialize(RFCallback *rfCallback);
	void process();
};


#endif