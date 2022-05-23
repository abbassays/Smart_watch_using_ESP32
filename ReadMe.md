# Modules
## Basic Modules
* [x] Blink ESP32 led
* [x] Connect SSD_1306 (0.96 inch) OLED with ESP32
* [x] Connect ESP32 to wifi
* [x] Make digital watch that has synced with the internet
## Feature Modules
* [x] Make webserver
* [x] Implement features in the app:
	* [x] sync alarm
	* [x] send SOS message
## Fun tasks done
* Display a monochrome image on SSD_1306 

# Overview
The ESP 32 is connected with the following components
* SSD1306 (0.96 inch)
* 330 ohm resistor
* Click button
* Peizo buzzer
* LED

The smart watch connects to WiFi and syncs time with it. A web server is used to set an alarm on the watch. The buzzer and led will than blink. On the press of a button the alarm is turned off. If the button is pressed for more than 3 seconds, an SOS email is sent. 
