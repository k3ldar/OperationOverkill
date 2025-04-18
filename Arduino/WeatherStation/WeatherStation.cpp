#include "api/Common.h"
#include "WeatherStation.h"

WeatherStation::WeatherStation(int dht11SensorSignalPin, int rainSensorAnalogPin, int lightSensorDigitalPin)
{
	_dht11 = new dht11;
	_dht11SensorSignalPin = dht11SensorSignalPin;
	_rainSensorAnalogPin = rainSensorAnalogPin;
	_lightSensorDigitalPin = lightSensorDigitalPin;
	pinMode(lightSensorDigitalPin, INPUT);
	_tempCelsius = -66.0;
    _humidity = -66.0;
	_nextUpdateTime = millis() + 5000;
}

WeatherStation::~WeatherStation()
{
  delete(_dht11);
}

void WeatherStation::initialize(SendMessageCallback *sendMesageCallback)
{
	_sendMessageCallback = sendMesageCallback;
}

void WeatherStation::process()
{
	unsigned long currTime = millis();

	if (currTime > _nextUpdateTime)
	{
		readRainSensor();
		readDHT11Sensor();
        readLightSensor();
		_nextUpdateTime = millis() + MillisBetweenSensorReading;
	}
}

void WeatherStation::readLightSensor()
{
	if (digitalRead(_lightSensorDigitalPin) == LOW)
	{
		Serial.println("Digital Light Sensor: ON");
		_dayTime = true;
	}
	else
	{
		_dayTime = false;
		Serial.println("Digital Light Sensor: OFF");
	}
}

void WeatherStation::readDHT11Sensor()
{
	_dht11->read(_dht11SensorSignalPin);

	Serial.print("Humidity: ");
	Serial.print(_dht11->humidity);
	Serial.print("; Temp: ");
	Serial.println(_dht11->temperature);
	_humidity = (float)_dht11->humidity;
	_tempCelsius = (float)_dht11->temperature;
	
	String tempHum = "Temp: ";
	tempHum.concat(_tempCelsius);
	tempHum.concat("(C); Humidity: ");
	tempHum.concat(_humidity);
	tempHum.concat("%");
	_sendMessageCallback(tempHum, Debug);
}

void WeatherStation::readRainSensor()
{
	int sensorValue = analogRead(_rainSensorAnalogPin);
	_rainSensorValue = map(sensorValue, 0, 1023, 255, 0); // map the 10-bit data to 8-bit data

    Serial.print("Rain Sensor: ");
    Serial.println(_rainSensorValue);

    if (_sendMessageCallback != NULL)
    {
		String msg = "Rains sensor value: ";
		msg.concat(_rainSensorValue);
		_sendMessageCallback(msg, Debug);
	}
}

float WeatherStation::getTemperature()
{
    return _tempCelsius;
}

float WeatherStation::getHumidity()
{
    return _humidity;
}

int WeatherStation::getRainSensor()
{
    return _rainSensorValue;
}

bool WeatherStation::getIsDayTime()
{
    return _dayTime;
}

