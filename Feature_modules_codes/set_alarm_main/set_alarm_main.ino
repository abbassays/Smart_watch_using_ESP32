// This code will light an LED (as an alarm) on the selected time

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// digital pin 4 has a pushbutton attached to it. Give it a name:
int pushButton = 4;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char *ssid = "Qamar Abbas Shah";
const char *password = "Alishah786";

const char *alarm_time_parameter = "alarmTime";

// HTML web page to take time as a input
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Enter a time to set alarm: <input type="datetime-local" name="alarmTime">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void BlinkLed(int LED_BUILTIN, int time /* blink led for how long */) {
  pinMode(LED_BUILTIN, OUTPUT);
  int time_count = 0;
  while (time_count < time && !digitalRead(pushButton)) {
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(500);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
    delay(500);                      // wait for a second
    time_count++;
    Serial.print("Blinking led for ");
    Serial.print(time_count);
    Serial.print("  Button state: ");
    Serial.println(digitalRead(pushButton));
  }
}

String inputMessage;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Pak time is 5 hours ahead of GMT so 5 * 3600s
  timeClient.setTimeOffset(5 * 3600);

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    String alarm_time;
    // GET alarmTime value on <ESP_IP>/get?alarmTime=<inputMessage>
    if (request->hasParam(alarm_time_parameter)) {
      inputMessage = request->getParam(alarm_time_parameter)->value();
      alarm_time = alarm_time_parameter;
      inputMessage = inputMessage + ":00Z";
    } else {
      inputMessage = "No message sent";
      alarm_time = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", "Alarm set for " + inputMessage + "<br>Current time: " + timeClient.getFormattedDate() + "<br><a href=\"/\">Reset Alarm</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  formattedDate = timeClient.getFormattedDate();

  if (formattedDate == inputMessage) {
    Serial.println("ALARM GOING OFF");
    // Blink led/buzzer for 5 secs
    BlinkLed(32, 60);
  }
}