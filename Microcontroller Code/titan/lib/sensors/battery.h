#ifndef TITAN_BATTERY_HEADER
#define TITAN_BATTERY_HEADER

#include <Arduino.h>

extern int FBatt;            // Front battery %
extern int RBatt;            // Rear battery %

void setupBattery();
void checkBatteries();

#endif
