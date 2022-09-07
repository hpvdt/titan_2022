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

extern byte humidity;       // Humidity (RH% * 2)
extern byte temperature;    // Temperature ((degC * 2) + 50)

void setupSensors();
void checkHDT();

#endif
