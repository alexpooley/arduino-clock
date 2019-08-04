
// https://github.com/arduino-libraries/NTPClient

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SERIAL_BAUD 115200  //make sure this is the same in arduino.ino

#define EPOCH 631123200

#define WIFI_SSID "brownbeagle"
#define WIFI_PASS "0438313337"



WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);


void sendEpochTime()
{
  String resp = "~";
         resp += timeClient.getEpochTime();
         resp += "|";
  Serial.print(resp);
}


void setup() {
  Serial.begin(SERIAL_BAUD);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  timeClient.begin();
  timeClient.update();

  while (timeClient.getEpochTime() < EPOCH) {
    timeClient.update();
    delay(1000);
  }
  
  Serial.print("~READY|");
}


void loop() {

  while(Serial.available()){
    char c = Serial.read();

    if ( c == '|'){
      timeClient.update();
      sendEpochTime();
    }
  }

  delay(500);
}
