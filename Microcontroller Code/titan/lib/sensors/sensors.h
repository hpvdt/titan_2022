#ifndef TITAN_SENSORS_HEADER
#define TITAN_SENSORS_HEADER

#include <Arduino.h>
#include "DHT.h" // Used for temperature and humidity (Adafruit DHT library)

#include "gpscode.h"
#include "encoder.h"
//#include "encodertimer.h"
#include "battery.h"
#include "co2.h"
#include "brakes.h"

extern byte humidity;          // Humidity
extern byte temperature;      // Temperature

void setupSensors();
void checkHDT();

#endif
