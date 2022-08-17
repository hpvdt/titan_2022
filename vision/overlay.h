#ifndef OVERLAY_CFILE_H
#define OVERLAY_CFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <stdbool.h>

#include "bcm_host.h"
#include "vgfont.h"

#include "revision.h"

void updateOverlayFront(float spe, float dist, int pow, int cad, int hr, float perf, float fbrake, int batt);
void updateOverlayRear(float spe, float dist, int rpow, int fpow, int cad, int hr, float fbrake, float rbrake, int batt, float perf, int CO2);

void startOverlay(bool cameraOn);

void closeOverlay();


///////////////////////////////////////////////////////
// Constants and variables



#endif
