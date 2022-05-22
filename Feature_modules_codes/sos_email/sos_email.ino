#include <WiFi.h>

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

// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------
void readButtonState();
void send_email(const char *event);

// Auxiliary variables (it will only detect changes that are 1500 milliseconds apart)
unsigned long previousMillis = 0;
const long interval = 1500;

const char *ssid = "Qamar Abbas Shah";
const char *password = "Alishah786";
const char *host = "maker.ifttt.com";
const char *apiKey = "dQru8C5MlOVIjwd_AvbTbjNaodmKPTopBzA0sX25zY7";

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(buttonPin, INPUT); // set buttonPin as input
  Serial.println("Press button");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop()
{
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