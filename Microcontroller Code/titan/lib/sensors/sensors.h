#ifndef TITAN_SENSORS_HEADER
#define TITAN_SENSORS_HEADER

#include <Arduino.h>
#include <Adafruit_MLX90614.h>
#include "DHT.h" // Used for temperature and humidity (Adafruit DHT library)

#include "gpscode.h"
#include "encoder.h"
#include "battery.h"
#include "co2.h"

extern byte humidity;          // Humidity
extern byte temperature;      // Temperature

// Brake temperatures
extern float frontBrakeTemp;
extern float rearBrakeTemp;

void setupSensors();
void checkHDT();
void checkBrakeTemps();

#endif
