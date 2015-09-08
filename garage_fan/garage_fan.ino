#include <dht.h>

boolean fanPower = false;
const int fanPin = 3;

// Temperature sensor (DHT22, aka: AM2302)
#define DHT22_PIN 2
double tempF, coldTemp = 1000, hotTemp = -1000;
const double START_TEMP = 72.0; // temperature (Fahrenheit) must be at or below this level for the fan to start running.
boolean cooling;

dht DHT;

void setup() {
  Serial.begin(9600);
  pinMode(fanPin, OUTPUT);
  
  // Use a base temperature reading and compare it to a temperature reading
  // some time in the future to determine if its heating up or cooling down. This
  // will tell us what cycle/"time" of the day it is.
  updateTemp();
  double baseTemp = tempF;
  double tolerance = 1.25;

  Serial.println("Base Temp: " + String(baseTemp));

  // Waiting for the temperature to change a certain amount to determine
  // if it is heating up or cooling down.
  while(abs(baseTemp - tempF) < tolerance) {
    Serial.println("Temp checked. " + String(tempF) + "F");

    // Wait just over two seconds (the sensor can only be read about once every two seconds)
    delay(2100);

    // Gather another temp sample
    updateTemp();
  }

  // There is now a significant difference in temperature
  // Now checking if it is heating up or cooling down outside
  if(baseTemp - tempF > 0) cooling = true;
  else cooling = false;
  delay(2100);
}

void loop() {
  updateTemp();
  updateFan();
  delay(2100);

  //printing out values
  Serial.print("Temp: " + String(tempF));
  Serial.print("\tLow: " + String(coldTemp));
  Serial.print("  High: " + String(hotTemp));
  String s = cooling ? "true" : "false";
  Serial.print("\tCooling: " + s);
  String s2 = digitalRead(fanPin) == HIGH ? "ON" : "OFF";
  Serial.println("  Fan: " + s2);
}

void updateFan() {
  if(cooling) { // Outside is cooling down
    if(digitalRead(fanPin) == HIGH) { // Fan is on
      // Update coldest temperature
      if(tempF < coldTemp) coldTemp = tempF;
      
      // Checking if temperature is heating up
      double tolerance = 1.0;
      if(tempF >= coldTemp + tolerance) { // Outside is now heating back up
        digitalWrite(fanPin, LOW);
        coldTemp = 1000;
        cooling = false;
      }
    }
    else { // Fan is off
      // Checking if temperature is low enough
      if(tempF <= START_TEMP) digitalWrite(fanPin, HIGH);
    }
    
  }
  else { // Outside is heating up
     // Update hottest temperature
      if(tempF > hotTemp) hotTemp = tempF;
      
      // Checking if temperature is cooling down
      double tolerance = 1.0;
      if(tempF <= hotTemp - tolerance) { // Outside is now cooling back down
        hotTemp = -1000;
        cooling = true;
      }
  }
}

void updateTemp() {
  // READ DATA
  int chk = DHT.read22(DHT22_PIN);
  switch (chk)
  {
    case DHTLIB_OK:
      tempF = (DHT.temperature * (9.0/5.0)) + 32;
      break;
    case DHTLIB_ERROR_CHECKSUM: 
      Serial.println("Checksum error"); 
      break;
    case DHTLIB_ERROR_TIMEOUT: 
      Serial.println("Time out error"); 
      break;
    default: 
      Serial.println("Unknown error");
      break;
  }
  if(chk != DHTLIB_OK) tempF = 999;
}
