#define WEATHER_STATION 

#include "Common.h"

#include <Arduino.h>
#include <WiFiS3.h>
#include "WebClientManager.h"
#include "LedManager.h"
#include "arduino_secrets.h" 
#include "SerialCommandManager.h"
#include "WeatherStation.h"

#include <ArduinoJson.h>

#define NIGHT_TIME_RELAY_PIN D8
#define TEMP_SENSOR_PIN D7
#define RAIN_SENSOR_ANALOG_PIN A0
#define LIGHT_SENSOR_ANALOG_PIN A1

#define UPDATE_SERVER_MILLISECONDS 5000

#define TIME_BETWEEN_LIGHT_PIN_CHANGE_MILLISECONDS 30000
#define LIGHT_SENSOR_DAYTIME 40

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

#ifdef DEBUG
	char server[] = "192.168.8.201";
	uint16_t port = 7100;
#endif
#ifdef RELEASE
	char server[] = "192.168.8.200";
	uint16_t port = 80;
#endif

long DeviceId = -1;

WebClientManager webClient;
SerialCommandManager commandMgr(&CommandReceived, '\n', ':', '=', 500, 256);
WeatherStation weatherStation(TEMP_SENSOR_PIN, RAIN_SENSOR_ANALOG_PIN, LIGHT_SENSOR_ANALOG_PIN);
LedManager ledManager;

unsigned long _lastFailureMessageSent = 0;
unsigned long _nextSendUpdate = 0;
unsigned long _nextLightPinChange = 0;
bool _dayTimePinActive = false;



void SendMessage(String message, MessageType messageType)
{
  switch (messageType)
  {
    case Debug:
      commandMgr.sendDebug(message, "WS");
      return;
    case Error:
      commandMgr.sendError(message, "WS");
      return;
    default:
      commandMgr.sendCommand("WS", message);
      return;
  }
}

void CommandReceived()
{
  if (commandMgr.getCommand() == "WIFI")
  {
    commandMgr.sendCommand("WIFI", webClient.wifiStatus());
    return;
  }

  if (commandMgr.getCommand() == "DID")
  {
    commandMgr.sendCommand("DID", String(DeviceId));
    return;
  }

  if (commandMgr.getCommand() == "RSSI")
  {
    commandMgr.sendCommand("RSSI", String(WiFi.RSSI()));
    return;
  }

  if (commandMgr.getCommand() == "WDBG0")
  {
    modem.noDebug();
    return;
  }

  if (commandMgr.getCommand() == "WDBG1")
  {
    modem.debug(Serial, 1);
    return;
  }

  if (commandMgr.getCommand() == "WDBG2")
  {
    modem.debug(Serial, 2);
    return;
  }

  if (commandMgr.getCommand() == "WCF")
  {
    commandMgr.sendCommand("WCF", String(webClient.wifiConnectFailures()));
    return;
  }
}

void setup()
{
    delay(200);
    Serial.begin(115200);
    while (!Serial);
    
    

    modem.timeout(500);
    webClient.initialize(&SendMessage, 10000, ssid, pass, WIFI_FAILURES_FOR_RESTART);
    webClient.setTimeout(500);
    weatherStation.initialize(&SendMessage);

    ledManager.Initialize();
    ledManager.StartupSequence();
    ledManager.SetIsRaining(false);
    pinMode(NIGHT_TIME_RELAY_PIN, OUTPUT);
}

void loop()
{
    commandMgr.readCommands();
    weatherStation.process();
    unsigned long currMillis = millis();

    webClient.process(currMillis);
    processLightChange(currMillis);

    if (DeviceId == -1)
    {
        registerDevice(currMillis);
    }
    else
    {
        process(currMillis);
    }

    processFailures(currMillis);
    ledManager.ProcessLedMatrix(&webClient, currMillis);
}

unsigned long lastLight = 0;
void processLightChange(unsigned long currMillis)
{
    if (currMillis > lastLight)
    {
        lastLight = currMillis + 1000;
        String current = _dayTimePinActive ? "Daytime" : "Nighttime";
        Serial.println(current);
    }
    if (currMillis > _nextLightPinChange)
    {
        bool isDayTime = weatherStation.getLightSensor() < LIGHT_SENSOR_DAYTIME;

        if (isDayTime == _dayTimePinActive)
        {
           return;
        }

        _dayTimePinActive = isDayTime;
        if (_dayTimePinActive)
        {
            Serial.println("Daytime");
            digitalWrite(NIGHT_TIME_RELAY_PIN, LOW);
        }
        else
        {
            Serial.println("Nighttime");
            digitalWrite(NIGHT_TIME_RELAY_PIN, HIGH);
        }

        _nextLightPinChange = currMillis + TIME_BETWEEN_LIGHT_PIN_CHANGE_MILLISECONDS;
    }
}

void process(unsigned long currMillis)
{
    if (currMillis > _nextSendUpdate)
    {
        updateServer(currMillis);          
    }
}

void registerDevice(unsigned long currMillis)
{
    if (DeviceId > -1)
        return;

    if (webClient.isWifiConnected())
    {
        if (webClient.getRequestSent() && webClient.getHasResponse())
        {
            int bufferSize = 500;
            char buffer[bufferSize];
            int bytesRead = webClient.getReadResponse(buffer, bufferSize);

            String debugMsg = combineStrings("Data Length: %d", bytesRead);
            SendMessage(debugMsg, Debug);

            if (bytesRead > 0)
            {
                JsonResponse response = webClient.htmlParseJsonBody(String(buffer));
                
                String debugMsg = combineStrings("Json Success: %d; Response Data: %s", response.success, String(response.json).c_str());
                SendMessage(debugMsg, Information);

                if (response.success)
                {
                    DeviceId = response.json.toInt();
                }
                else
                {
                    SendMessage("Failed to register device", Error);
                }

                String deviceMsg = combineStrings("Device Id: %ld", DeviceId);
                SendMessage(deviceMsg, Information);
            }

        }
        else if (!webClient.getRequestSent())
        {
            char tempPath[30];
            sprintf(tempPath, "/Device/RegisterDevice/%d/", DeviceWeatherStation);
            webClient.get(currMillis, server, port, tempPath);
        }
    }
}

void processFailures(unsigned long currMillis)
{
    // not required but reports failure
    int failures = webClient.socketConnectFailures();

    if (failures > 0 && currMillis > _lastFailureMessageSent)
    {
        _lastFailureMessageSent = currMillis + FAIL_REPORTING_MS;
        String failureCount = "Socket Failure Count: ";
        failureCount.concat(failures);
        SendMessage(failureCount, Information);
    }

    if (webClient.wifiConnectFailures() >= WIFI_FAILURES_FOR_RESTART || failures >= WIFI_FAILURES_FOR_RESTART)
    {
        SendMessage("Wifi/Socket connect Fail exceeded", Error);
        Serial2.write(RESET_WIFI, sizeof(RESET_WIFI) - 1);
        ledManager.ShutdownSequence();
        NVIC_SystemReset();
        SendMessage("!!!!!!!!!!!!!  this should not appear !!!!!!!!!", Error);
    }
}

void updateServer(unsigned long currMillis)
{
    if (webClient.isWifiConnected())
    {
        if (webClient.postRequestSent() && webClient.postHasResponse())
        {
            int bufferSize = 4000;
            char buffer[bufferSize];
            int bytesRead = 0;
            
            bytesRead = webClient.postReadResponse(buffer, bufferSize);
            String debugMsg = combineStrings("Data Length: %d", bytesRead);
            SendMessage(debugMsg, Debug);

            if (bytesRead > 0)
            {
                String data = String(buffer);

                JsonResponse response = webClient.htmlParseJsonBody(data);
                               
                String debugMsg = combineStrings("Json Success: %d; Response Data: %s", response.success, String(response.json).c_str());
                SendMessage(debugMsg, Information);

                if (response.success)
                    _nextSendUpdate = currMillis + UPDATE_SERVER_MILLISECONDS;
            }
        }
        else if (webClient.canConnectToSocket(currMillis) && !webClient.postRequestSent())
        {
            SendMessage("Sending post request", Debug);
            commandMgr.sendCommand("WIFI", webClient.wifiStatus());
            float temp = weatherStation.getTemperature();
            float humid = weatherStation.getHumidity();
            float rainSensor = weatherStation.getRainSensor();
            bool rain = rainSensor > 200;
            bool light = weatherStation.getLightSensor() < LIGHT_SENSOR_DAYTIME;
            ledManager.SetIsRaining(rain);
            ledManager.SetTemperature(temp);
            ledManager.SetHumidity(humid);
            char tempPath[80];
            sprintf(tempPath, "/Device/UpdateWeather/%ld/%f/%f/%f/%d/%d/", DeviceId, temp, humid, rainSensor, rain, light);
            webClient.post(currMillis, server, port, tempPath);
        }
    }
    else
    {
        SendMessage("Wifi not connected", Error);
    }
}