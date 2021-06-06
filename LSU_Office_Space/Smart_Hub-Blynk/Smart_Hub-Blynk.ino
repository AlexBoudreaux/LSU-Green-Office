/***************************************************
  Wiring: 
  Screen -> Wemos

  Vcc, Reset, LED -> 3.3V
  GND -> GND
  CS -> D8
  MOSI -> D7
  SCK -> D5
 ****************************************************/


#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <RotaryEncoder.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h>



    // Wi-Fi 
//    const char ssid[] = "CollegeHillStudios-2G";
//    const char password[] = "drpepperisawoman";

    const char ssid[] = "Connectify-3315";
    const char password[] = "PFT12345";

    // Blynk Auth Token
    const char auth[] = "snw4x8DFsPBUM1-TGdOskgWl6rqw_IAQ";

    WidgetLCD lcd(V3);

    // Backend Address
    const String httpAddress = "http://6485a83c58cc.ngrok.io";

    // MAC Address
    String  MAC = WiFi.macAddress();

    // Device Creds
    String deviceID;
    String buildingRoom;
    String deviceType;
    String desk;

    // Device Variables
    int heartBeatMin = 5;
    int tempResetMins = 1;

    int minutes = 0;
    bool amPm = true;

    enum colors{
      darkBlue = 194,
      lightBlue = 217,
      white = 240,
      lightRed = 263,
      darkRed = 286
      // 0x0218, 0x2E5F, 0xFFFF, 0xFAAA, 0xE000
    };

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

#define PIN_IN1 D1
#define PIN_IN2 D2

#define PIN_IN3 D0
#define PIN_IN4 D6

const long utcOffsetInSeconds = -21600;

char daysOfTheWeek[7][12] = {"Sun.", "Mon.", "Tues.", "Wed.", "Thurs.", "Fri.", "Sat."};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "north-america.pool.ntp.org", utcOffsetInSeconds);


Adafruit_ILI9341 tft = Adafruit_ILI9341(15, 2, 13, 14, 15, 12);


RotaryEncoder encoder1(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoder2(PIN_IN3, PIN_IN4, RotaryEncoder::LatchMode::FOUR3);

ICACHE_RAM_ATTR void checkPosition1()
{
  encoder1.tick(); // just call tick() to check the state.
}

ICACHE_RAM_ATTR void checkPosition2()
{
  encoder2.tick(); // just call tick() to check the state.
}




void setup() {

  Blynk.begin(auth, ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
  
  Serial.begin(115200);
  Serial.println("\n******** Smart Hub ********\n"); 

  attachInterrupt(PIN_IN1, checkPosition1, FALLING);
  attachInterrupt(PIN_IN2, checkPosition1, FALLING);

  attachInterrupt(PIN_IN3, checkPosition2, FALLING);
  attachInterrupt(PIN_IN4, checkPosition2, FALLING);
 
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);

  
  // UI

  //  Knob Icons
  tft.fillCircle(70,  220, 15, 0xFFFF);  // Draw circle (x,y,radius,color)
  tft.fillRect(55, 205, 32, 15, 0x0000);
  tft.fillCircle(67,  220, 11, 0xFFFF);  // Draw circle (x,y,radius,color)
  tft.drawCircle(70,220,15, 0xFFFF);
  tft.drawCircle(70,220,16, 0xFFFF);

  tft.fillCircle(250, 220, 15, 0xFFFF);  // Draw circle (x,y,radius,color)
  tft.fillRect(235, 205, 32, 15, 0x0000);
  tft.fillCircle(247,  220, 11, 0xFFFF);  // Draw circle (x,y,radius,color)
  tft.drawCircle(250,220,15, 0xFFFF);
  tft.drawCircle(250,220,16, 0xFFFF);

  //   + & - 's
  tft.fillRoundRect(32,  220, 15, 3, 1, 0xFFFF);  // Draw rounded rectangle (x,y,width,height,radius,color)
  tft.fillRoundRect(93,  220, 15, 3, 1, 0xFFFF);
  tft.fillRoundRect(99,  214, 3, 15, 1, 0xFFFF);
  tft.fillRoundRect(212, 220, 15, 3, 1, 0xFFFF);  // Draw rounded rectangle (x,y,width,height,radius,color)
  tft.fillRoundRect(273, 220, 15, 3, 1, 0xFFFF);
  tft.fillRoundRect(279, 214, 3, 15, 1, 0xFFFF);

  //  Light Bar
  tft.drawRect(14,  85, 110, 115, 0xFFFF);

  //  Temp Bars
  tft.fillRoundRect(194, 83, 20, 112, 3, 0x0218);
  tft.drawRoundRect(194, 83, 20, 112, 3, 0xFFFF);
  tft.fillRoundRect(217, 83, 20, 112, 3, 0x2E5F);
  tft.drawRoundRect(217, 83, 20, 112, 3, 0xFFFF);
  tft.fillRoundRect(240, 83, 20, 112, 3, 0xFFFF);
  tft.fillRoundRect(245, 95, 20-10, 112-24, 3, 0x0000);
  tft.fillRoundRect(263, 83, 20, 112, 3, 0xFAAA);
  tft.drawRoundRect(263, 83, 20, 112, 3, 0xFFFF);
  tft.fillRoundRect(286, 83, 20, 112, 3, 0xE000);
  tft.drawRoundRect(286, 83, 20, 112, 3, 0xFFFF);

  //  Lables
  tft.setCursor(30, 60);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setTextSize(2);
  tft.println("Lights:");

  tft.setCursor(225, 60);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setTextSize(2);
  tft.println("Temp:");

  minutes = timeClient.getMinutes();

  tft.setCursor(10, 6);
  tft.setTextColor(0xFFFF, 0x0000);  tft.setTextSize(3);

  lcd.clear();
  lcd.print(0, 0, "I am Comfortable.");

}

void loop() {

  Blynk.run();

  static int pos1 = 0;
  static int pos2 = 0;

  static int light = 0;

  static int curs1 = 198;
  static int curs2 = white;

  encoder1.tick();
  encoder2.tick();

  timeClient.update();

  

  if(timeClient.getMinutes() != minutes){
    setTime();
  }


  int newPos1 = encoder1.getPosition();
  if (pos1 != newPos1) {
    switch ((int)(encoder1.getDirection())) {
      case -1:
        if(!(curs1 > 187)){
          tft.fillRect(14+1, curs1, 112-5, 11-2, 0x0000);
          curs1 += 11;
          Blynk.virtualWrite(V2, curs1);
        }
        break;
      case 0:
        break;
      case 1:
        if(!(curs1 <= 83)){
          curs1 -= 11;
          tft.fillRect(14, curs1, 112, 11, 0xFFFF);
          Blynk.virtualWrite(V2, curs1);
        }
        break;
    }
    pos1 = newPos1;
  }

  
  int newPos2 = encoder2.getPosition();
  if (pos2 != newPos2) {
    switch ((int)(encoder2.getDirection())) {
      case -1:
        if(!(curs2 <= darkBlue)){
          colors s = (colors)(curs2);
          uint16_t color;
          switch (s){
            case lightBlue:
              color = 0x2E5F;
              lcd.clear();
              lcd.print(0, 0, "I am Very Cold!");
              break;
            case white:
              color = 0xFFFF;
              lcd.clear();
              lcd.print(0, 0, "I am Cold.");
              break;
            case lightRed:
              color = 0xFAAA;
              lcd.clear();
              lcd.print(0, 0, "I am Comfortable.");
              break;
            case darkRed:
              color = 0xE000;
              lcd.clear();
              lcd.print(0, 0, "I am Hot.");   
              break;      
          }
          tft.fillRoundRect(curs2+1, 83+1, 20-2, 112-2, 3, color);
          curs2 -= 23;
          tft.fillRoundRect(curs2+5, 83+12, 20-10, 112-24, 3, 0x0000);
        }
        break;
      case 0:
        break;
      case 1:
        if(!(curs2 >= darkRed)){
          colors s = (colors)(curs2);
          uint16_t color = 0xFFFF;
          switch (s){
            case darkBlue:
              color = 0x0218;
              lcd.clear();
              lcd.print(0, 0, "I am Cold.");
              break;
            case lightBlue:
              color = 0x2E5F;
              lcd.clear();
              lcd.print(0, 0, "I am Comfortable.");
              break;
            case white:
              color = 0xFFFF;
              lcd.clear();
              lcd.print(0, 0, "I am Hot.");
              break;
            case lightRed:
              color = 0xFAAA;
              lcd.clear();
              lcd.print(0, 0, "I am Very Hot!");
              break;
          }
          tft.fillRoundRect(curs2+1, 83+1, 20-2, 112-2, 3, color);
          curs2 += 23;
          tft.fillRoundRect(curs2+5, 83+12, 20-10, 112-24, 3, 0x0000);
        }
        break;
    }
  }
}



void setTime(){

  tft.setCursor(10, 6);
  tft.setTextColor(0xFFFF, 0x0000);  tft.setTextSize(3);
  tft.print(daysOfTheWeek[timeClient.getDay()]);
  tft.setCursor(180,6);
  int hours = timeClient.getHours();
  hours += 1;     // Day Light Savings
  amPm = true;
  if (hours > 12){ hours -= 12; amPm = false;}
  else if (hours == 12){ amPm = false;} 
  else if (hours == 0){ hours = 12; amPm = true;}
  tft.print(hours);
  tft.print(":");
  minutes = timeClient.getMinutes();
  if(minutes < 10){
    tft.print("0");
    tft.print(minutes);
  }else{
    tft.print(minutes);
  }
  if(amPm){
    tft.print("am");
  }else{
    tft.print("pm");
  }
 
}
