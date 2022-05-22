#include <WiFi.h>
#include <time.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1); 

const char* ssid = "Qamar Abbas Shah";
const char* password = "Alishah786";
 
int GMTOffset = 18000;  //Replace with your GMT Offset in seconds
int daylightOffset = 0;  // Replace with your daylight savings offset in seconds
 
void setup() { 
Serial.begin(115200); 

if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
  Serial.println(F("SSD1306 allocation failed"));
  for(;;);
}
  delay(2000);
display.clearDisplay();
display.setTextSize(1); 
display.setCursor(0,0);
display.setTextColor(WHITE);

WiFi.begin(ssid, password);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected to Wi-Fi!");

configTime(GMTOffset, daylightOffset, "pool.ntp.org","time.nist.gov");
}

 
void loop() {
time_t rawtime = time(nullptr);
struct tm* timeinfo = localtime(&rawtime);

Serial.print("Time: ");
Serial.print(timeinfo->tm_hour);
Serial.print(":");
Serial.print(timeinfo->tm_min);
Serial.print(":");
Serial.println(timeinfo->tm_sec);

display.clearDisplay();
display.setTextSize(3);
display.setTextColor(WHITE);
display.setCursor(0,25);
if( timeinfo->tm_hour <10)
display.print("0");
display.print(timeinfo->tm_hour);
display.print(":");

if( timeinfo->tm_min <10)
display.print("0");
display.print(timeinfo->tm_min);
 
display.setTextSize(2);
display.setCursor(75,30);
display.print(":");
if( timeinfo->tm_sec <10)
display.print("0");
display.print(timeinfo->tm_sec); 
display.display();
 
delay(1000); 
 
}