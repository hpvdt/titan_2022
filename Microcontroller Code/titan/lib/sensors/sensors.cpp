#include "sensors.h"

byte humidity = 0;          // Humidity
byte temperature = 50;      // Temperature

// Digital Humidity and Temperature setup
const byte DHTPin = PB5;  // DHT sensor pin (temperature and humidity)
DHT dht(DHTPin, DHT22); // Sets up sensor
const unsigned int dhtPeriod = 2000; // Period between measurements (updates only once every few seconds)

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