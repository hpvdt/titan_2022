#ifndef TITAN_CO2_HEADER
#define TITAN_CO2_HEADER

#include <Arduino.h>

extern volatile int CO2ppm;  // CO2 level in ppm

void setupCO2();

#endif