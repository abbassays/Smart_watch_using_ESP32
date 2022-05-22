#include <Arduino.h>
// Library includedd to connect to WiFi
#include <WiFi.h>

// Libraries included to host a server
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Libraries included to get time from internet
#include <NTPClient.h>
#include <WiFiUdp.h>

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char *ssid = "Qamar Abbas Shah";
const char *password = "Alishah786";

const char *input_parameter = "alarmtime";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
// Variable to save date and time
String live_time;
String alarm_time;

// HTML web page to handle 3 input fields (input1, input2, alarmtime)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <h1 style="text-align: centre;" >Set Alarm on ESP32</h1>
    <form action="GET">
        <label for="alarmtime">Choose time to set alarm (date and time):</label>
        <input type="datetime-local" id="alarmtime" name="alarmtime">
        <input type="submit">
    </form>
    
</body>
</html>)rawliteral";

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void setup()
{



    // Connect to WiFi
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

    // Send web page with input fields to client
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html); });

    // Send a GET request to <ESP_IP>/get?input1=<alarm_time>
    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String inputParam;

    // GET alarmtime value on <ESP_IP>/get?alarmtime=<alarm_time>
    if (request->hasParam(input_parameter)) {
      alarm_time = request->getParam(input_parameter)->value();
      inputParam = input_parameter;
    //  append alarm_time to match live_time format
      alarm_time = alarm_time + ":00Z";

    }
    else {
      alarm_time = "No message sent";
      inputParam = "none";
    }
    Serial.println(alarm_time);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + alarm_time +
                                     "<br><a href=\"/\">Return to Home Page</a>"); });
    server.onNotFound(notFound);
    server.begin();

    // Initialize a NTPClient to get time
    timeClient.begin();
    // Set offset time in seconds to adjust for your timezone, for example:
    // GMT +1 = 3600
    // GMT +8 = 28800
    // GMT -1 = -3600
    // GMT 0 = 0
    timeClient.setTimeOffset(5 * 3600);
}

void loop()
{
    // while (!timeClient.update())
    // {
    //     timeClient.forceUpdate();
    // }
    // // The live_time comes with the following format:
    // // 2018-05-28T16:00:13Z
    // live_time = timeClient.getFormattedDate();

    // if (live_time == alarm_time)
    // {
    //     Serial.println("ALARM GOING OFF");
    //     delay(25000);
    // }
    // else
    // {
    //     Serial.println(live_time);
    //     Serial.println(alarm_time);
    //     Serial.println("");
    // }
    // delay(1000);
}