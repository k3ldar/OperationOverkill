//#include "Common.h"

void(* resetFunc) (void) = 0;


#include <Arduino_LED_Matrix.h>
#include "WebClientManager.h"
#include "arduino_secrets.h" 
#include "SerialCommandManager.h"
#include "WeatherStation.h"

#include <ArduinoJson.h>

#define TEMP_SENSOR_PIN 7
#define RAIN_SENSOR_ANALOG_PIN A0

#define UPDATE_SERVER_MILLISECONDS 5000
#define FAIL_REPORTING_MS 1000

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)


//char server[] = "192.168.8.200";
//uint16_t port = 80;

char server[] = "192.168.8.201";
uint16_t port = 7100;

long DeviceId = -1;

WebClientManager webClient;
SerialCommandManager commandMgr(&CommandReceived, '\n', ':', '=', 500, 256);
WeatherStation weatherStation(TEMP_SENSOR_PIN, RAIN_SENSOR_ANALOG_PIN);
ArduinoLEDMatrix matrix;

unsigned long _nextSendUpdate = 0;
unsigned long _lastFailureMessageSent = 0;

unsigned long _nextLedUpdate = 0;

byte ledFrame[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};
long rssiRate[8] = { -20, -30, -40, -50, -60, -65, -70, -80};

void UpdateConnectedState()
{
    int status = webClient.getWiFiStatus();

    if (status == WL_CONNECTED)
    {
        for (int i = 3; i < 9; i++)
        {
            ledFrame[6][i] = 1;
            ledFrame[7][i] = 1;
        }

    }
    else if (status == WL_CONNECTING)
    {
        for (int i = 3; i < 9; i++)
        {
            ledFrame[7][i] = 1;
        }
    }
    else 
    {
        for (int i = 3; i < 9; i++)
        {
            ledFrame[6][i] = 0;
            ledFrame[7][i] = 0;
        }
    }
}

void UpdateFailureCount(int failures)
{
    for (int i = 3; i < 8; i++)
        ledFrame[0][i] = (failures + 3) > i ? 1 : 0;
}
void UpdateWebCommunication(bool isSending)
{
    for (int i = 0; i < 8; i++)
        ledFrame[i][11] = isSending ? 1 : 0;
}

void UpdateSignalStrength()
{
    long rssi = webClient.getRssi();

    for (int i = 0; i < 8; i++)
        ledFrame[i][0] = rssiRate[i] < rssi ? 1 : 0;

    if (rssi == 0)
    {
        ledFrame[1][0] = 0;
        ledFrame[3][0] = 0;
        ledFrame[5][0] = 0;
        ledFrame[7][0] = 0;
    }
}

void ProcessLedMatrix(unsigned long currMillis)
{
    int failures = webClient.socketConnectFailures();
    UpdateFailureCount(failures);

    if (currMillis > _nextLedUpdate)
    {
        UpdateSignalStrength();
        UpdateConnectedState();
        _nextLedUpdate = currMillis + 300;
    }
    matrix.renderBitmap(ledFrame, 8, 12);
}

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
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);
    
    matrix.begin();

    modem.timeout(500);

    webClient.initialize(&SendMessage, 10000, ssid, pass);
    webClient.setTimeout(500);
    weatherStation.initialize(&SendMessage);
}

void loop()
{
    commandMgr.readCommands();
    weatherStation.process();

    unsigned long currMillis = millis();

    webClient.process(currMillis);

    if (DeviceId == -1)
    {
        registerDevice(currMillis);
    }
    else
    {
        process(currMillis);
    }

    bool hasDelay = false;
    processFailures(currMillis);

    if (webClient.getRequestSent() || webClient.postRequestSent())
    {
        UpdateWebCommunication(true);
    }
    else
    {
        UpdateWebCommunication(false);
        hasDelay = true;
    }

    ProcessLedMatrix(currMillis);

    if (hasDelay)
        delay(50);
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
        String failureCount = "Failure Count: ";
        failureCount.concat(failures);
        SendMessage(failureCount, Information);
    }

    if (webClient.wifiConnectFailures() > 20)
    {
        SendMessage("Wifi connect Fail exceeded", Information);
        //resetFunc();
        webClient.connectToWiFi();
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
        else if (webClient.canConnectToSocket(currMillis) && !webClient.postRequestSent())
        {
            Serial.println("Sending post request");
            commandMgr.sendCommand("WIFI", webClient.wifiStatus());
            float temp = weatherStation.getTemperature();
            float humid = weatherStation.getHumidity();
            float rainSensor = weatherStation.getRainSensor();
            bool rain = false;
            char tempPath[80];
            sprintf(tempPath, "/Device/UpdateWeather/%ld/%f/%f/%f/%d/", DeviceId, temp, humid, rainSensor, rain);
            webClient.post(currMillis, server, port, tempPath);
        }
    }
    else
    {
        Serial.println("Wifi not connected");
    }
}