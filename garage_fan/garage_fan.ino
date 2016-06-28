#include <dht.h>

boolean fanPower = false;
const int fanPin = 2;

double externalTemp, internalTemp, coldTemp = 1000;
//const double SETPOINT = 70.0; // temperature (Fahrenheit) must be at or below this level for the fan to start running.
boolean cooling;

// Temperature sensors (DHT22, aka: AM2302)
dht EXTERNAL_DHT, INTERNAL_DHT;
#define INTERNAL_PIN 3
#define EXTERNAL_PIN 4

void setup() {
  Serial.begin(9600);
  pinMode(fanPin, OUTPUT);
  delay(1000);
  Serial.println("\n\n");
  
  // Ensure fan is off
  digitalWrite(fanPin, LOW);
}

void loop() {
  externalTemp = getTemp(EXTERNAL_DHT, EXTERNAL_PIN);
  internalTemp = 70.0; // Will be changed when sensor is added to circuit
  updateFan();
  displayValues();
  
  waitMin(5);
}

double getTemp(dht DHT, int pin) {
  // READ DATA
  int chk = DHT.read22(pin);
  switch (chk)
  {
    case DHTLIB_OK:
      return (DHT.temperature * (9.0/5.0)) + 32.0;
      break;
    case DHTLIB_ERROR_CHECKSUM: 
      Serial.println("Checksum error"); 
      break;
    case DHTLIB_ERROR_TIMEOUT: 
      Serial.println("Time out error"); 
      break;
    default: 
      Serial.println("Unknown error");
  }
  return 999;
}

void updateFan() {
  // Take note of fan status
  boolean fanPower = digitalRead(fanPin) == HIGH;
  
  // The fan only runs if the difference between internal and external temperatures are greater than +2.0 degrees
  double diff = internalTemp - externalTemp;
  if(diff >= 2.0) {
    // Recording the lowest temperature
    if(externalTemp < coldTemp) coldTemp = externalTemp;
    
    // Now checking to see if the temperature is heating up, the fan will run only when
    // the temperature is currently dropping. Once the temperature begins to rise the fan
    // will be shut off.
    double tolerance = 1.75;
    cooling = (externalTemp <= coldTemp + tolerance);

    // Air is cooling down
    if(cooling) {
      // If fan is off, turn it on
      if(!fanPower) digitalWrite(fanPin, HIGH);
    }
    // Air is heating up
    else {
      // If fan is on, turn it off
      if(fanPower) digitalWrite(fanPin, LOW);
    }
  }
  else {
    // If fan is on, turn it off
    if(fanPower) digitalWrite(fanPin, LOW);
    coldTemp = 1000;
  }
}

void displayValues() {
  // Displaying values
  Serial.print("External Temp: " + String(externalTemp));
  Serial.print("\tInternal Temp: " + String(internalTemp));
  Serial.print("\tLow Temp: " + String(coldTemp));
  String s = cooling ? "true" : "false";
  Serial.print("\t|\tCooling: " + s);
  String s2 = digitalRead(fanPin) == HIGH ? "ON" : "OFF";
  Serial.println("\tFan: " + s2);
}

void waitMin(int minutes) {
  for(int x = 0; x < minutes; x++) {
    // Wait 1 minute
    for(int n = 0; n < 60; n++) {
      // Wait 1 second
      delay(1000);
    }
  }
}
