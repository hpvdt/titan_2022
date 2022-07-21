#ifndef BIKE_HEADER
#define BIKE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>

#include "bcm_host.h"
#include "vgfont.h"

#include "revision.h"

#include "serialComs.h"
#include "overlay.h"
#include <time.h>

float CIRCUMFERENCE = 2.104;

int cadence = 0; 
int power = 0;
int heartRate = 0;
int battery = 0;
int rotations = 0;
float speed = 0.0;
float distance = 0.0;
float temperature = 0.0;
float humidity = 0.0;

bool useSerial = true; // Use serial or not
int serialLine = 0;

int numberFrames = 10000; // Number of frames to render for testing

#endif
