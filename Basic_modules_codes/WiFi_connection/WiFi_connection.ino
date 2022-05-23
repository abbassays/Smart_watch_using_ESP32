#include "WiFi.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#define WIFI_USERNAME "Qamar Abbas Shah" // not actual username and password 
#define WIFI_PASSWORD "Alishah786"
#define TIMEOUT 20000 // timeout after 20 secs


//function prototype
void setupDisplay();

void connectToWiFi()
{
  // display something
  display.println("Connecting to WiFi...");
  display.display();

  // wifi set up
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_USERNAME, WIFI_PASSWORD);

  // store the start time of attempt
  unsigned long attemptStartTime = millis();

  while (WiFi.status() != WL_CONNECTED && TIMEOUT > millis() - attemptStartTime)
  {
  // keep trying to connect while until timeout reached
  }

  if (WiFi.status() != WL_CONNECTED)
  {// error message if not connected
    display.println("Failed to connect :(");
  }
  else
  {// else success message
    display.println("Successfully connected to:");
    display.println(WiFi.localIP());
  }
  display.display();
}

void setupDisplay()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
  // clear display before starting
  display.clearDisplay();
  display.display();

  //set text size, color, position
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  setupDisplay();
  connectToWiFi();
}

void loop()
{
  // put your main code here, to run repeatedly:
}
