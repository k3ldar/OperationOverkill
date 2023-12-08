#include "WeatherStation.h"

WeatherStation::WeatherStation(const int tempSensorSignalPin)
{
	_tempSensorSignalPin = tempSensorSignalPin;
	_oneWire = new OneWire(_tempSensorSignalPin);
	_tempSensor = new DallasTemperature(_oneWire);
	_currentTemperature = -66.0;
	_nextUpdateTime = millis();
}

WeatherStation::~WeatherStation()
{

}

void WeatherStation::initialize(RFCallback *rfCallback)
{
	_rfCallback = rfCallback;
	_tempSensor->begin();
}

void WeatherStation::process()
{
	unsigned long currTime = millis();

	if (currTime > _nextUpdateTime)
	{
		if (_currentTemperature < -10)
			_currentTemperature += 0.5;
		else
			_currentTemperature = -56.7;

		String updateTemp = "2/T1/" +
			String(_currentTemperature);
		Serial.println(updateTemp);
		_rfCallback(updateTemp);
		Serial.print("Device Count");
		Serial.println(_tempSensor->getDeviceCount());
		_tempSensor->requestTemperatures();
		_tempCelsius = _tempSensor->getTempCByIndex(0);
		Serial.print("Temperature: ");
		Serial.print(_tempCelsius);
		Serial.println("°C");

		Serial.println(digitalRead(_tempSensorSignalPin));
		_nextUpdateTime = millis() + MillisBetweenSensorReading;
	}
}