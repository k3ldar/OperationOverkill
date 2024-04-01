#include "WaterPump.h"

WaterPump::WaterPump(
			  const int sensorSignalPin,
			  const int sensorActivePin,
			  const int sensorActiveLEDPin,
			  const int pump1LEDPin,
			  const int pump2LEDPin,
			  const int pump1Pin,
			  const int pump2Pin)
{
	_queue = new Queue(15);
	_sensorSignalPin = sensorSignalPin;
	_sensorActivePin = sensorActivePin;
	_sensorActiveLEDPin = sensorActiveLEDPin;
	_pump1LEDPin = pump1LEDPin;
	_pump2LEDPin = pump2LEDPin;
	_pump1Pin = pump1Pin;
	_pump2Pin = pump2Pin;
	
	_sensorValue = 0;
	_myTime = 0;
	_nextTemperatureCheck = millis();
}

WaterPump::~WaterPump()
{
	Serial.println("WaterPump destroy");
	delete(_queue);
}
 
void WaterPump::initialize(SendMessageCallback *sendMessageCallback)
{
	_sendMessageCallback = sendMessageCallback;
	_currentTemperature = TemperatureInitialValue;
	pinMode(_sensorActivePin, OUTPUT);
	pinMode(_sensorActiveLEDPin, OUTPUT);
	pinMode(_pump1LEDPin, OUTPUT);
	pinMode(_pump2LEDPin, OUTPUT);
	pinMode(_pump1Pin, OUTPUT);
	pinMode(_pump2Pin, OUTPUT);

	digitalWrite(_sensorActivePin, LOW);
	digitalWrite(_sensorActiveLEDPin, LOW);
	digitalWrite(_pump1LEDPin, LOW);
	digitalWrite(_pump2LEDPin, LOW);
	digitalWrite(_pump1Pin, LOW);
	digitalWrite(_pump2Pin, LOW);
}

double WaterPump::temperatureGet()
{
	return _currentTemperature;
}

void WaterPump::temperatureSet(double temperature)
{
	if (_currentTemperature != temperature)
	{
		_currentTemperature = temperature < MinimumWorkingTemperature ? TemperatureNotSet : temperature;
		_nextTemperatureCheck = millis() + ValidTemperatureLength;
	}
}

void WaterPump::process(unsigned long currMillis)
{
	if (currMillis > _nextTemperatureCheck)
	{
		_sendMessageCallback("Temperature not set", Debug);
		temperatureSet(TemperatureNotSet);
        turnPump1Off();
        turnPump2Off();
	}
	
    bool validateSensor = false;
    
    if (currMillis - _myTime > ReadSensorMs)
        validateSensor = true;
	
	if (validateSensor)
    {
        _myTime = currMillis;
		_sendMessageCallback("Validating Sensor Value", Debug);
    
        int s1Value = getSensorValue();

        if (_queue->isFull())
          _queue->dequeue();
          
        _queue->enqueue(s1Value);
		
        if (s1Value != _sensorValue)
        {
            _sensorValue = s1Value;
        }

        _sensorAverage = _queue->average();

        if (_queue->isFull() && _currentTemperature >= MinimumWorkingTemperature)
        {
          processPump1(currMillis, _sensorAverage);
          processPump2(currMillis, _sensorAverage);
        }

        if (_currentTemperature < MinimumWorkingTemperature && _pump1Active)
            turnPump1Off();

        if (_currentTemperature < MinimumWorkingTemperature && _pump2Active)
            turnPump2Off();

        String combined = "1/WS/" + String(s1Value) + "/" + 
          String(_sensorAverage) + "/" +
          String(_currentTemperature) + "/" + 
          String(_pump1Active) + "/" +
          String(_pump2Active);

        _sendMessageCallback(combined, Information);	
    }
}

int WaterPump::getSensorValue()
{
    digitalWrite(_sensorActiveLEDPin, HIGH);
    digitalWrite(_sensorActivePin, HIGH);

    delay(10);
    int s1Value = analogRead(_sensorSignalPin);
	
    digitalWrite(_sensorActivePin, LOW);
    digitalWrite(_sensorActiveLEDPin, LOW);

    return s1Value;
}


void WaterPump::processPump1(unsigned long currTime, int average)
{
    if (_pump1Active)
    {
        if (currTime >= _turnPump1OffMilli || _sensorValue <= MinimumCutoffThreshold)
        {
          turnPump1Off();
        }
    }
    else if (average >= Relay_1_Threshold)
    {
        turnPump1On();
    }	
}

void WaterPump::turnPump1On()
{
    digitalWrite(_pump1LEDPin, HIGH);
    digitalWrite(_pump1Pin, HIGH);
    Serial.println("Relay 1 ON");
    _pump1Active = true;
    _turnPump1OffMilli = millis() + MinimumPumpTimeMs;
}

void WaterPump::turnPump1Off()
{
    if (!_pump1Active)
        return;
        
    digitalWrite(_pump1LEDPin, LOW);
    digitalWrite(_pump1Pin, LOW);
    Serial.println("Relay 1 OFF");
    _pump1Active = false;
}

bool WaterPump::pump1Active()
{
	return _pump1Active;
}

void WaterPump::processPump2(unsigned long currTime, int average)
{    
	if (_pump2Active)
    {
        if (currTime > _turnPump2OffMilli || (_pump2Active && !_pump1Active) || _sensorValue < MinimumCutoffThreshold || _sensorValue < Relay_2_Threshold)
        {
          turnPump2Off();
        }
    }
    else if (average > Relay_2_Threshold)
    {
        turnPump2On();
    }
}

void WaterPump::turnPump2On()
{
    if (!_pump2Active)
      return;
      
    digitalWrite(_pump2LEDPin, HIGH);
    digitalWrite(_pump2Pin, HIGH);
    Serial.println("Pump 2 ON");
    _pump2Active = true;
    _turnPump2OffMilli = millis() + MinimumPumpTimeMs;
}

void WaterPump::turnPump2Off()
{
    digitalWrite(_pump2LEDPin, LOW);
    digitalWrite(_pump2Pin, LOW);
    Serial.println("Pump 2 OFF");
    _pump2Active = false;
}

bool WaterPump::pump2Active()
{
	return _pump2Active;
}

int WaterPump::sensorValue()
{
    return _sensorValue;
}

int WaterPump::sensorAverage()
{
    return _sensorAverage;
}
