#include <ArduinoJson.h>

#include "Common.h"
#include "WebClientManager.h"
#include "arduino_secrets.h" 
#include "SerialCommandManager.h"

#include "WaterPump.h"


// water sensor pins
#define WaterSensor_Signal_Pin A4
#define Sensor_Active_LED_PIN 2
#define Relay_1_LED_PIN 3
#define Relay_2_LED_PIN 4
#define Relay_1_PIN 5
#define Relay_2_PIN 6
#define WaterSensor_Power_Pin 7

#define TEMP_UPDATE_MAX 1000 * 60 * 60

#define TEMPERATURE_RETRIEVE_MILLISECONDS 60000
#define UPDATE_SERVER_MILLISECONDS 800


char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

//char server[] = "192.168.8.200";
//uint16_t port = 80;

char server[] = "192.168.8.201";
uint16_t port = 7100;
long DeviceId = -1;

WebClientManager webClient;
SerialCommandManager commandMgr(&CommandReceived, '\n', ':', '=', 500, 512);
WaterPump waterPump(WaterSensor_Signal_Pin, WaterSensor_Power_Pin, Sensor_Active_LED_PIN, Relay_1_LED_PIN, Relay_2_LED_PIN, Relay_1_PIN, Relay_2_PIN);

unsigned long _nextGetTempUpdate = 0;
unsigned long _nextSendUpdate = 0;

void SendMessage(String message, MessageType messageType)
{
    switch (messageType)
    {
        case Debug:
            commandMgr.sendDebug(message, "WP");
            return;
        case Error:
            commandMgr.sendError(message, "WP");
            return;
        default:
            commandMgr.sendCommand("WP", message);
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

  if (commandMgr.getCommand() == "TMP")
  {
    commandMgr.sendCommand("TMP", String(waterPump.temperatureGet()));
    return;
  }

  if (commandMgr.getCommand() == "P1")
  {
    commandMgr.sendCommand("P1", String(waterPump.pump1Active()));
    return;
  }

  if (commandMgr.getCommand() == "P2")
  {
    commandMgr.sendCommand("P2", String(waterPump.pump2Active()));
    return;
  }}

void setup()
{
    Serial.begin(230400);
    while (!Serial);
  
    modem.timeout(500);
    webClient.initialize(&SendMessage, 10000, ssid, pass);
    waterPump.initialize(&SendMessage);
}

void loop()
{
    commandMgr.readCommands();
     
    unsigned long currMillis = millis();
    
    waterPump.process(currMillis);

    webClient.process();

    if (DeviceId == -1)
    {
        registerDevice(currMillis);
    }
    else
    {
        getTemperature(currMillis);

        if (currMillis > _nextSendUpdate)
        {
            updateServer(currMillis);
        }
    }

    processFailures();

    if (!webClient.getRequestSent() && !webClient.postRequestSent())
        delay(50);
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

            Serial.print("Data Length: ");
            Serial.println(bytesRead);

            if (bytesRead > 0)
            {
                Serial.println(String(buffer, bytesRead));

                JsonResponse response = webClient.htmlParseJsonBody(String(buffer));
                
                Serial.print("Json Success: ");
                Serial.println(response.success);
                Serial.print("Response Data: ");
                Serial.println(response.json);

                if (response.success)
                {
                    DeviceId = response.json.toInt();
                }
                else
                {
                    Serial.println("Failed to register device");
                }

                Serial.print("Device Id: ");
                Serial.println(DeviceId);
            }
        }
        else if (!webClient.getRequestSent())
        {
            char tempPath[30];
            sprintf(tempPath, "/Device/RegisterDevice/%d/", DeviceWaterPump);
            webClient.get(currMillis, server, port, tempPath);
        }
    }
}

void processFailures()
{
    // not required but reports failure
    int failures = webClient.socketConnectFailures();

    if (failures > 0)
    {
        String failureCount = "Failure Count: ";
        failureCount.concat(failures);
        SendMessage(failureCount, Information);
    }
}


void getTemperature(unsigned long currMillis)
{
    if (currMillis > _nextGetTempUpdate && webClient.isWifiConnected())
    {
        if (webClient.getRequestSent() && webClient.getHasResponse())
        {
            int bufferSize = 4000;
            char buffer[bufferSize];
            int bytesRead = 0;
            
            bytesRead = webClient.getReadResponse(buffer, bufferSize);
            Serial.print("Data Length: ");
            Serial.println(bytesRead);

            if (bytesRead > 0)
            {
                Serial.println(String(buffer, bytesRead));

                String data = String(buffer);

                JsonResponse response = webClient.htmlParseJsonBody(data);
                
                Serial.print("Json Success: ");
                Serial.println(response.success);
                Serial.print("Response Data: ");
                Serial.println(response.json);

                if (response.success)
                {
                    _nextGetTempUpdate = currMillis + TEMPERATURE_RETRIEVE_MILLISECONDS;

                    JsonDocument temperatureJson;
                    deserializeJson(temperatureJson, response.json);

                    double temperature = static_cast<double>(temperatureJson["Temperature"].as<float>());
                    Serial.print("Temp from Json: ");
                    Serial.println(temperature);

                    unsigned long age = temperatureJson["AgeMilliSeconds"].as<unsigned long>();
                    Serial.print("Age From Json: ");
                    Serial.println(age);

                    if (age > TEMP_UPDATE_MAX)
                    {
                        waterPump.temperatureSet(TemperatureNotSet);
                        Serial.println("Temp data too old, resetting");
                    }
                    else
                    {
                        waterPump.temperatureSet(temperature);
                        Serial.println("Temp data age is good");
                    }
                }
            }
        }
        else if (!webClient.getRequestSent())
        {
            char tempPath[50];
            sprintf(tempPath, "/Device/GetCurrentTemperature/%ld/", DeviceId);
            webClient.get(currMillis, server, port, tempPath);
        }
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
            Serial.print("Data Length: ");
            Serial.println(bytesRead);

            if (bytesRead > 0)
            {
                Serial.println(String(buffer, bytesRead));

                String data = String(buffer);

                JsonResponse response = webClient.htmlParseJsonBody(data);
                
                Serial.print("Json Success: ");
                Serial.println(response.success);
                Serial.print("Response Data: ");
                Serial.println(response.json);

                if (response.success)
                    _nextSendUpdate = currMillis + UPDATE_SERVER_MILLISECONDS;
            }
        }
        else if (!webClient.postRequestSent() && !webClient.getRequestSent())
        {
            Serial.println("Sending post request");
            float temp = waterPump.temperatureGet();
            int value = waterPump.sensorValue();
            int average = waterPump.sensorAverage();
            bool pump1Active = waterPump.pump1Active();
            bool pump2Active = waterPump.pump2Active();
            
            char tempPath[80];
            sprintf(tempPath, "/Device/UpdateWaterPump/%ld/%d/%d/%f/%d/%d/", DeviceId, value, average, temp, pump1Active, pump2Active);
            webClient.post(currMillis, server, port, tempPath);
        }
    }
    else
    {
        Serial.println("Wifi not connected");
    }
}