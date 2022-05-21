// This software uses several open software,
// with each of different license declaration.
// Please use this software with understanding of
// each included software license
// Script by kinoene.jp

#include <NTPClient.h>  //For NTP Time Sync
#include <WiFi.h>       //For WiFi Network Connection
#include <WiFiUdp.h>    //For WiFi UDP
#include <MD_Parola.h>  //For MAX7219 Matrix LED
#include <MD_MAX72xx.h> //For MAX7219 SPI LED Driver
#include <SPI.h>        //For SPI Communication
#include <time.h>       //For Time Based Application 
#include "GF3x8p.h"     // 3x8 font
#include "GF5x8p.h"     // 5x8 font

//Wi-Fi
#define SSID "********"     //Use your own WiFi SSID
#define WIFIKEY "********"  //Ise your own WiFi Key

//NTP client
#define ntpServer "***.***.***.***" //NTP IP Address; Set your local NTP
#define tzOffset 32400     // JST = 3600 * 9; Set your local offset to UTC in second

//MAX7219
#define MAX_DEVICES 4 // four modules
#define CLK_PIN   18
#define DATA_PIN  23
#define CS_PIN    5

#define SPEED_TIME  25  //Small numbers are faster. Zero is the fastest.

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, tzOffset, 43200000); 
  //12 hours : 12(hour) x 60(min) x 60(sec) x 1000(ms) = 43200000 for NTP Sync renewal

void setup() {
//WiFi Connection Setting
WiFi.begin(SSID, WIFIKEY);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
  }

  timeClient.begin();

// Matrix Display Setting
  P.begin(2); // 2 zones
  P.setZone(0, 1, 3);  //00011111 <- Zone 0  for HM display
  P.setZone(1, 0, 0);  //11100000 <- Zone 1  for Sec display
  P.setFont(0,GF5x8p);
  P.setFont(1,GF3x8p);
  P.setIntensity(0);  //Darkest
}

void loop() {

  time_t epTime = timeClient.getEpochTime();
  struct tm  ts;
  char bufS[3], bufT[5]; // bufS = Sec, 'ss' + 1 = 3,  bufT = HourMinute, 'hhmm" + 1 = 5
  ts = *localtime(&epTime);

  timeClient.update();

  P.displayAnimate();

//Zone 0 Time
  if (P.getZoneStatus(0)) {
    strftime(bufT, sizeof(bufT), "%H%M", &ts); // %T: hh:mm:ss
    P.displayZoneText(0, bufT, PA_LEFT, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayReset(0);
  }
//Zone 1 Time
  if (P.getZoneStatus(1)) {
    strftime(bufS, sizeof(bufS), "%S", &ts); // %T: hh:mm:ss
    P.displayZoneText(1, bufS, PA_RIGHT, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayReset(1);
  }
  delay(50); //Don't remove this delay, and don't make it too small
}
