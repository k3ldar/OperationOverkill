#include "Common.h"

#include <HCSR04.h>

//#include "WebClientManager.h"
//#include "arduino_secrets.h" 
#include "SerialCommandManager.h"
//#include "WeatherStation.h"

//#include <ArduinoJson.h>

#define TEMP_SENSOR_PIN 7
#define RAIN_SENSOR_ANALOG_PIN A0
#define BUZZER_PIN 10

#define UPDATE_SERVER_MILLISECONDS 5000

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
//char ssid[] = SECRET_SSID;        // your network SSID (name)
//char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)


//char server[] = "192.168.8.200";
//uint16_t port = 80;

//char server[] = "192.168.8.201";
//uint16_t port = 7100;

long DeviceId = -1;

HCSR04 hc(2, 3);
//WebClientManager webClient;
SerialCommandManager commandMgr(&CommandReceived, '\n', ':', '=', 500, 256);
//WeatherStation weatherStation(TEMP_SENSOR_PIN, RAIN_SENSOR_ANALOG_PIN);

//unsigned long _nextSendUpdate = 0;
unsigned long flipBuzzerState = 0;
unsigned long nextSr04Measurement = 0;
unsigned long nextToneChange = 0;
int currentTone = 0;

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
    //commandMgr.sendCommand("WIFI", webClient.wifiStatus());
    return;
  }

  if (commandMgr.getCommand() == "DID")
  {
    commandMgr.sendCommand("DID", String(DeviceId));
    return;
  }
}


void setup()
{
    Serial.begin(230400);
    while (!Serial);

    //modem.timeout(500);

    //webClient.initialize(&SendMessage, 10000, ssid, pass);
    //weatherStation.initialize(&SendMessage);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
}

void loop()
{
    commandMgr.readCommands();
    //weatherStation.process();

    unsigned long currMillis = millis();

    bool isBuzzerOn = digitalRead(BUZZER_PIN) == HIGH;

    if (currMillis > flipBuzzerState)
    {

        if (isBuzzerOn == LOW)
        {
            //digitalWrite(BUZZER_PIN, HIGH);
            //Serial.println("Turning buzzer on");
        }
        else if (isBuzzerOn == HIGH)
        {
            //digitalWrite(BUZZER_PIN, LOW);
            //Serial.println("Turning buzzer off");
        }

        flipBuzzerState = currMillis + 50;
    }

    if (isBuzzerOn && currMillis > nextToneChange)
    {
        nextToneChange = currMillis + 800;
        //UpdateTone();
    }

    if (currMillis > nextSr04Measurement)
    {
        float distance = hc.dist();
        nextSr04Measurement = currMillis + 100;
        Serial.print("Distance is: ");
        Serial.print(distance);
        Serial.println(" cm");

        if (isBuzzerOn && distance > 50)
        {
            digitalWrite(BUZZER_PIN, LOW);
        }
        else if (!isBuzzerOn && distance > 0 && distance < 50)
        {
            currentTone = 0;
            digitalWrite(BUZZER_PIN, HIGH);
        }
    }

    /*
    webClient.process();

    if (DeviceId == -1)
    {
        registerDevice(currMillis);
    }
    else
    {
        process(currMillis);
    }


    processFailures();

    if (!webClient.getRequestSent() && !webClient.postRequestSent())
        delay(50);
    */


}

void UpdateTone()
{
    if (currentTone == 0)
    {
        tone(BUZZER_PIN, 440); // A4
        currentTone = 1;
    }
    else if (currentTone == 1)
    {
        tone(BUZZER_PIN, 494); // B4
        currentTone = 2;
    }
    else if (currentTone == 2)
    {
        tone(BUZZER_PIN, 523); // C4
        currentTone = 3;
    }
    else if (currentTone == 3)
    {
        tone(BUZZER_PIN, 587); // D4
        currentTone = 4;
    }
    else if (currentTone == 4)
    {
        tone(BUZZER_PIN, 659); // E4
        currentTone = 5;
    }
    else if (currentTone == 5)
    {
        tone(BUZZER_PIN, 698); // F4
        currentTone = 6;
    }
    else if (currentTone == 6)
    {
        tone(BUZZER_PIN, 784); // G4
        currentTone = 0;
    }
}

/*

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
        else if (!webClient.postRequestSent())
        {
            Serial.println("Sending post request");
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

*/