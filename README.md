An Arduino bsed clock that will display for 30 minutes to a TFT screen and then turn off until motion detected with the PIR.

Time is updated via NTP on boot and after wake up from hibernation.

This project is a lot of sample code glued together. Not indicative of how I normally work :)

Hardware used:
- Arduino Uni Wifi (with the onboard ESP 8266)
- Linker shield
- Linker cable
- KeyStudio PIR
- 320x240 TFT screen

The ESP8266 is used to connect to WiFi and pull time from NTP when it receives a serial command ending with | character.

The Arduino will wait for a ~READY| serial signal from the ESP8266 and then request the time via ~SYNC|

Relevant third party docs in docs folder.

PIR info here: https://wiki.keyestudio.com/Ks0052_keyestudio_PIR_Motion_Sensor
ESP WiFi samples from https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html