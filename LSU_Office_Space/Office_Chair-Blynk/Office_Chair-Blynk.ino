

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>
#include <iostream>
#include <string>


/*    Office Chair - with Blynk Backend
 *    Created By: Alex Boudreaux
 * 
 */


// Wi-Fi 
const char ssid[] = "Connectify-3315";
const char pass[] = "PFT12345";

// Blynk Auth Token
const char auth[] = "snw4x8DFsPBUM1-TGdOskgWl6rqw_IAQ";

WidgetLED led1(V1);


// Device Creds
String deviceID;
String buildingRoom;
String deviceType;
String desk;

// Device Variables
bool occupied = 0;
const int sleepSeconds = 5;

// Seat Sensor 
const int seatSensorPin = D1;
const int battSensorPin = A0;
int sensorValue = 0;
    



 
void setup() {

    Serial.begin(115200);                                   // Serial connection
    
    pinMode(D0, WAKEUP_PULLUP);
    pinMode(battSensorPin, INPUT);
    pinMode(seatSensorPin, INPUT);

    bool curOcc = checkCur();                               // Return true or false if someone is in the seat
    connectToWiFi();
    patchOccupied(curOcc);                                  // Send that true or flase to Blynk Backend

    float volt = analogRead(battSensorPin)/10.23;
    int voltInt = (int)volt; 
    String v = String(voltInt);
    v += " %";
    Blynk.virtualWrite(V7, v);                              // Send battery & to Blynk Backend

    delay(1000);
    ESP.deepSleep(sleepSeconds * 1000000);                  // Go to Deep Sleep

}




void loop() {  
  Blynk.run();
}




bool checkCur(){
    // Check if the seat sensor is currently activated 

    sensorValue = digitalRead(seatSensorPin);

    if (sensorValue){              // Seat Occupied 
      Serial.println("Got Here Seat True");
      return true;
    }else{
      Serial.println("Got Here Seat False");
      return false;
    }
}





bool patchOccupied(bool occupied){

   if (WiFi.status() == WL_CONNECTED) {                      //Check WiFi connection status
 
    if (occupied){
      led1.on();
    } else {
      led1.off();
    }
  } 
}






void connectToWiFi(){

    Serial.println();
    Serial.println();
    Serial.println();
    
    Blynk.begin(auth, ssid, pass);
    
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println("Connecting...");
      delay(500);
    } 
    
    Serial.println();
    Serial.print("Connected to: ");
    Serial.print(ssid);
    Serial.println();  
   
}
