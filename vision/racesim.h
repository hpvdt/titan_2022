#ifndef RACE_SIM_HEADER
#define RACE_SIM_HEADER

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

void RaceSimV3_WHPSC_complete(float xpStart, bool recordSimulation); // Runs the entire simulation
void setupRaceSim(); // Used to set all the values
float compareToSimulation (float speed, float position, float power); // Called to estimate performance to simulation

#endif