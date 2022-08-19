#include "sensors.h"

byte humidity = 0;          // Humidity
byte temperature = 50;      // Temperature
float frontBrakeTemp = 0;   // Front brake temperature
float rearBrakeTemp = 0;    // Rear brake temperature


// Digital Humidity and Temperature setup
const byte DHTPin = PB5;  // DHT sensor pin (temperature and humidity)
DHT dht(DHTPin, DHT22); // Sets up sensor
const unsigned int dhtPeriod = 2000; // Period between measurements (updates only once every few seconds)


Adafruit_MLX90614 frontBrake = Adafruit_MLX90614();
Adafruit_MLX90614 rearBrake = Adafruit_MLX90614();
const byte frontBrakeAdd = 0x01;
const byte rearBrakeAdd = 0x02;
const float frontEmissivity = 1.0;
const float rearEmissivity = 1.0;
const unsigned long brakePeriod = 1000;


inline void setupBrakeThermometers() {
  if (frontBrake.begin(frontBrakeAdd)) {
#ifdef ALLOW_DEBUG_SERIAL
    if (debugMode) {
      DEBUGSERIAL.print("Error connecting to FRONT brake sensor at address: ");
      DEBUGSERIAL.println(frontBrakeAdd);
    }
#endif
  }
  if (rearBrake.begin(rearBrakeAdd)) {
#ifdef ALLOW_DEBUG_SERIAL
    if (debugMode) {
      DEBUGSERIAL.print("Error connecting to REAR brake sensor at address: ");
      DEBUGSERIAL.println(rearBrakeAdd);
    }
#endif
  }

  frontBrake.writeEmissivity(frontEmissivity);
  rearBrake.writeEmissivity(rearEmissivity);
}

void getBrakeTemps() {
  frontBrakeTemp = frontBrake.readObjectTempC();
  rearBrakeTemp = rearBrake.readObjectTempC();
}

void checkBrakeTemps() {
  static unsigned long brakeTime = 0;
  
  if (millis() > brakeTime) {
    getBrakeTemps();
    
    brakeTime = millis() + brakePeriod;
  }
}

void setupSensors() {
  setupBattery();
  setupEncoder();
  setupCO2();
  setupBrakeThermometers();

  dht.begin(); // Starts digital humidity and temperature sensor
}

void checkHDT() {
  static unsigned long dhtTime = 0;

  // Periodic DHT measurement
  if (millis() > dhtTime) {
    humidity = dht.readHumidity() * 2;            // Humidity reading
    temperature = 50 + dht.readTemperature() * 2; // Temperature reading

    dhtTime = millis() + dhtPeriod; // Sets next measurement time
  }
}