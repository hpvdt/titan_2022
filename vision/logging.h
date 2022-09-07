#ifndef BIKE_LOGGING_HEADER
#define BIKE_LOGGING_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <dirent.h> // Needed for counting files

void startLogging();
void updateLog(float spd, float dist, int fpwr, int rpwr, int fcad, int rcad, int fhr, int rhr, float temperature, float hum, float fbat, float rbat, float fbrk, float rbrk, int CO2, float perf, float gpsSpd);


#endif
