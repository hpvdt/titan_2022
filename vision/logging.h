#ifndef BIKE_LOGGING_HEADER
#define BIKE_LOGGING_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void startLogging();
void updateLog(float spd, float dist, int fpwr, int rpwr, int fcad, int rcad, int fhr, int rhr, float temperature, float hum, float fbat, float rbat);


#endif