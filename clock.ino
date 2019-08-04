
// A lot of code copied from the MCUFRIEND_kbv example.

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#include <SPI.h>          // f.k. for Arduino-1.5.2
#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h>
#include <avr/sleep.h>            // Library for putting our arduino into sleep modes
#include "PinChangeInterrupt.h"

#define SENSOR_PIN 11

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define SERIAL_BAUD 115200

// Seconds since Jan 1, 1990
#define EPOCH 631123200
// +8 hours
#define TIME_OFFSET 28800

// 10 mins
#define SHOW_TIME_FOR 600000
// 10 seconds
//#define SHOW_TIME_FOR 10000


MCUFRIEND_kbv tft;

uint16_t g_identifier;
long timeBaseline = EPOCH;

String serialBuffer;

long lastPirChange = 0;


void debugWrite(String str) {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.println(str);
}


void waitWifi() {
  while (serialBuffer != "READY") {
    serialBuffer = nextToken();
    delay(500);
  }
}

String nextToken()
{
  String buf;
  char lastChar = "";

  // Continue to read until sentinel.
  // Delay until sentinel is read.
  while (lastChar != "|") {
    while (Serial.available()) {
      lastChar = Serial.read();
  
      if (lastChar == '|'){
        return buf;
      } else if (lastChar == '~') {
        buf = "";
      } else {
        buf += lastChar;
      }
    }
    if (buf == "") {
      delay(100);
    }
  }
}


void syncTime() {
  Serial.println("~SYNC|");
  serialBuffer = nextToken();
  timeBaseline = serialBuffer.toInt() - (millis()/1000);
}


void hibernate() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // lowest power consumption mode 
  //"The Power-down mode saves the register contents but freezes the Oscillator, disabling all other chip functions 
  // until the next interrupt or hardware reset."  text from ATMEGA328P datasheet
  
  ADCSRA &= ~(1 << 7);   // Disable ADC - don't forget to flip back after waking up if you need ADC in your application ADCSRA |= (1 << 7);  (From Kevin's sketch)
    
  sleep_enable();                       // enable the sleep mode function
  sleep_bod_disable();                  //to disable the Brown Out Detector (BOD) before going to sleep. 

  sleep_mode();                // calls function to put arduino in sleep mode 
  sleep_disable();            // when interrupt is received, sleep mode is disabled and program execution resumes from here
//  disablePinChangeInterrupt(digitalPinToPinChangeInterrupt(SENSOR_PIN));

  // Sync time before sleeping.
  syncTime();
}

void pirChanged() {
  lastPirChange = millis();
}

void setup() {
  Serial.begin(SERIAL_BAUD); //same as ESP baud

  g_identifier = tft.readID(); //
  //Serial.println(g_identifier);
  if (g_identifier == 0x00D3 || g_identifier == 0xD3D3) g_identifier = 0x9481; // write-only shield
  if (g_identifier == 0xFFFF) g_identifier = 0x9341; // serial
  
  tft.begin(g_identifier);
  tft.setRotation(1);
  tft.setTextColor(WHITE);
  tft.setTextSize(6);
  tft.fillScreen(BLACK);

  debugWrite("Waiting for WiFi");
  waitWifi();
  debugWrite("Synchronising time");
  syncTime();

  debugWrite("Initialising sensor");
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(SENSOR_PIN), pirChanged, CHANGE);
  enablePinChangeInterrupt(digitalPinToPinChangeInterrupt(SENSOR_PIN));
  lastPirChange = millis();
}



void showTime() {
  long t = timeBaseline;
       t += (millis()/1000);
       t += TIME_OFFSET;
  long h = (t / 3600) % 12;
  t = t % 3600;
  int m = t / 60;
  int s = t % 60;

  char str[12];
  sprintf(str, "%02ld:%02d:%02d", h, m, s);
  
  tft.fillScreen(BLACK);
  tft.setCursor(20, 100);
  tft.println(str);
}

void loop() {
  interrupts();

  // Hide time after X minutes.
  while(millis() < (lastPirChange + SHOW_TIME_FOR)) {
    showTime();
    delay(900);
  }

  tft.fillScreen(BLACK);
  hibernate();

}
