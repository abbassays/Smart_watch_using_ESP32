// This code will light an LED (as an alarm) on the selected time

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char *ssid = "Qamar Abbas Shah";
const char *password = "Alishah786";

const char *PARAM_INPUT_3 = "input3";

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    input3: <input type="datetime-local" name="input3">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void BlinkLed(int LED_BUILTIN, int time /* blink led for how long */)
{
    pinMode(LED_BUILTIN, OUTPUT);
    int start_time = millis();
    while (millis() - start_time != time)
    {
        digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);                  // wait for a second
        digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
        delay(1000);                  // wait for a second
    }
}

String inputMessage;

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("WiFi Failed!");
        return;
    }
    Serial.println();
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Initialize a NTPClient to get time
    timeClient.begin();
    // Set offset time in seconds to adjust for your timezone, for example:
    // GMT +1 = 3600
    // GMT +8 = 28800
    // GMT -1 = -3600
    // GMT 0 = 0
    timeClient.setTimeOffset(5 * 3600);

    // Send web page with input fields to client
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html); });

    // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String inputParam;
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
      inputMessage = inputMessage + ":00Z";
    } else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", "Alarm set for " + inputMessage + "<br>Current time: " +  timeClient.getFormattedDate() +"<br><a href=\"/\">Reset Alarm</a>"); });
    server.onNotFound(notFound);
    server.begin();    
}

void loop()
{
    while (!timeClient.update())
    {
        timeClient.forceUpdate();
    }
    // The formattedDate comes with the following format:
    // 2018-05-28T16:00:13Z
    formattedDate = timeClient.getFormattedDate();

    if (formattedDate == inputMessage)
    {
        Serial.println("ALARM GOING OFF");
        BlinkLed(32,25000);
    }
}