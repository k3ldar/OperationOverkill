#ifndef __WeatherStation__H
#define __WeatherStation__H

//#include <Arduino.h>
//#include <Temperature_LM75_Derived.h>
#include <dht11.h>
#include "Common.h"

#define MillisBetweenSensorReading 1500;

class WeatherStation
{
private:
	dht11 *_dht11;
	int _dht11SensorSignalPin;
	float _tempCelsius;
    float _humidity;
    int _rainSensorValue;
	int _rainSensorAnalogPin;

	unsigned long _nextUpdateTime;
	void readRainSensor();
	void readDHT11Sensor();

	SendMessageCallback *_sendMessageCallback;
public:
	WeatherStation(int tempSensorSignalPin, int rainSensorAnalogPin);
	~WeatherStation();
	void initialize(SendMessageCallback *sendMesageCallback);
	void process();
    float getTemperature();
    float getHumidity();
    int getRainSensor();
};


#endif