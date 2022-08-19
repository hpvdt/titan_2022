#ifndef TITAN_BRAKES_HEADER
#define TITAN_BRAKES_HEADER

#include <Arduino.h>
#include <Adafruit_MLX90614.h>
#include "communication.h"

// Brake temperatures
extern float frontBrakeTemp;
extern float rearBrakeTemp;

void setupBrakeThermometers();
void checkBrakeTemps();

#endif