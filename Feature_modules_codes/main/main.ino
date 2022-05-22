// This code will light an LED (as an alarm) on the selected time

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// digital pin 4 has a pushbutton attached to it. Give it a name:
int pushButton = 4;
// LONG PRESS VARIABLES
static const int buttonPin = 4; // switch pin
int buttonStatePrevious = LOW;  // previousstate of the switch

unsigned long minButtonLongPressDuration = 3000; // Time we wait before we see the press as a long press
unsigned long buttonLongPressMillis;             // Time in ms when we the button was pressed
bool buttonStateLongPress = false;               // True if it is a long press

const int intervalButton = 50;      // Time between two readings of the button state
unsigned long previousButtonMillis; // Timestamp of the latest reading

unsigned long buttonPressDuration;

unsigned long currentMillis; // Variabele to store the number of milleseconds since the Arduino has started
// Variables to save date and time
String formattedDate;

void BlinkLed(int LED_BUILTIN, int time /* blink led for how long */);
void readButtonState();
void send_email(const char *event);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char *ssid = "Qamar Abbas Shah";
const char *password = "Alishah786";
const char *host = "maker.ifttt.com";
const char *apiKey = "dQru8C5MlOVIjwd_AvbTbjNaodmKPTopBzA0sX25zY7";

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

String inputMessage;

void setup()
{
  // start serial monitor
  Serial.begin(115200);
// start WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("Connected to IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Pak time is 5 hours ahead of GMT so 5 * 3600s
  timeClient.setTimeOffset(5 * 3600);

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
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
    request->send(200, "text/html", "Alarm set for " + inputMessage + "<br>Current time: " + timeClient.getFormattedDate() + "<br><a href=\"/\">Reset Alarm</a>"); });
  server.onNotFound(notFound);
  server.begin();
}

void loop()
{
  // update time constantly
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
    // Blink led/buzzer for 60 secs
    BlinkLed(32, 60);
  }
    currentMillis = millis(); // store the current time
  readButtonState();        // read the button state
}

// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
void readButtonState()
{
  if (currentMillis - previousButtonMillis > intervalButton)
  {
    // Read the digital value of the button (LOW/HIGH)
    int buttonState = digitalRead(buttonPin);
    // If the button has been pushed AND
    // If the button wasn't pressed before AND
    // IF there was not already a measurement running to determine how long the button has been pressed
    if (buttonState == HIGH && buttonStatePrevious == LOW && !buttonStateLongPress)
    {
      buttonLongPressMillis = currentMillis;
      buttonStatePrevious = HIGH;
      Serial.println("Button pressed");
    }
    // Calculate how long the button has been pressed
    buttonPressDuration = currentMillis - buttonLongPressMillis;
    // If the button is pressed AND
    // If there is no measurement running to determine how long the button is pressed AND
    // If the time the button has been pressed is larger or equal to the time needed for a long press
    if (buttonState == HIGH && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration)
    {
      buttonStateLongPress = true;
      Serial.println("Button long pressed");

      send_email("SOS_email");
    }
    // If the button is released AND
    // If the button was pressed before
    if (buttonState == LOW && buttonStatePrevious == HIGH)
    {
      buttonStatePrevious = LOW;
      buttonStateLongPress = false;
      Serial.println("Button released");
      // If there is no measurement running to determine how long the button was pressed AND
      // If the time the button has been pressed is smaller than the minimal time needed for a long press
      // Note: The video shows:
      //       if (!buttonStateLongPress && buttonPressDuration < minButtonLongPressDuration) {
      //       since buttonStateLongPress is set to FALSE on line 75, !buttonStateLongPress is always TRUE
      //       and can be removed.
      if (buttonPressDuration < minButtonLongPressDuration)
      {
        Serial.println("Button pressed shortly");
      }
    }
    // store the current timestamp in previousButtonMillis
    previousButtonMillis = currentMillis;
  }
}

void send_email(const char *event)
{
  Serial.print("Connecting to ");
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort))
  {
    Serial.println("Connection failed");
    return;
  }
  // Creating a URL for the request
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += apiKey;
  Serial.print("Requesting URL: ");
  Serial.println(url);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  while (client.connected())
  {
    if (client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    else
    {
      // No data yet, wait a bit
      delay(50);
    };
  }
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}

void BlinkLed(int LED_BUILTIN, int time /* blink led for how long */)
{
  pinMode(LED_BUILTIN, OUTPUT);
  int time_count = 0;
  while (time_count < time && !digitalRead(pushButton))
  {
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(500);                      // wait for a second
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
    delay(500);                      // wait for a second
    time_count++;
    Serial.print("Blinking led for ");
    Serial.print(time_count);
    Serial.print("  Button state: ");
    Serial.println(digitalRead(pushButton));
  }
}