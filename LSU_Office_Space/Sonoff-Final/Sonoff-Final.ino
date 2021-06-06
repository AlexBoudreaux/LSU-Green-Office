


#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <iostream>
#include <string>

#include "CSE7766.h"
CSE7766 myCSE7766;


// Wi-Fi 
const char ssid[] = "Connectify-3315";
const char password[] = "PFT12345";

// Blynk Auth Token
const char auth[] = "snw4x8DFsPBUM1-TGdOskgWl6rqw_IAQ";

// MAC Address
String  MAC = WiFi.macAddress();

// GPIOs
const int relayPin = 12;      // Sonoff relay & red led
const int led = 13;           // Sonoff blue led
const int buttonPin = 0;      // Sonoff On/Off button

// Check values every 2 seconds
const int updateTime = 2000;

// variables will change
int buttonState = LOW;          // variable for reading the pushbutton status
int lastButtonState = HIGH;     // previous state of the button
int relayState = LOW;
int ledState = LOW;
int volts = 0;
int relayStatus = 0;

long lastTime = 0;              // the last time the output pin was toggled
long debounce = 200;            // the debounce time, increase if the output flickers




void setup() {

  // Initialize
  myCSE7766.setRX(1);
  myCSE7766.begin();

  Serial.begin(115200);

  EEPROM.begin(512);

  pinMode(buttonPin, INPUT);    // Side Button
  pinMode(led, OUTPUT);         // Blue LED
  pinMode(relayPin, OUTPUT);    // Relay
  digitalWrite(relayPin, HIGH);

  pinMode(relayPin, INPUT_PULLUP); 
  attachInterrupt(digitalPinToInterrupt(relayPin), handleInterrupt, CHANGE); 

  Blynk.begin(auth, ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 100 );
  }
  
}




void loop() {

  Blynk.run();

  static unsigned long mLastTime = 0;

  if ((millis() - mLastTime) >= updateTime) {

        mLastTime = millis();

        myCSE7766.handle();

        Blynk.virtualWrite(V5, myCSE7766.getVoltage()); //sending to Blynk
        Blynk.virtualWrite(V4, myCSE7766.getCurrent()); //sending to Blynk

     }

}




void handleInterrupt() { 
    if (buttonState == HIGH && lastButtonState == LOW && millis() - lastTime > debounce) {
      if (relayState == HIGH) {
        relayState = LOW;
      }
      else {
        relayState = HIGH;
      }

      lastTime = millis();
    }

  digitalWrite(relayPin, relayState); // Relay open/close - digital pin 12
  lastButtonState = buttonState;
}
