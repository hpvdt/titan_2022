#ifndef TITAN_BRAKES_HEADER
#define TITAN_BRAKES_HEADER

#include <Arduino.h>

// Brake temperatures
extern float frontBrakeTemp;    // Front brake temperature (deg C)
extern float rearBrakeTemp;     // Rear brake temperature (deg C)

void setupBrakeThermometers();
void checkBrakeTemps();

#endif