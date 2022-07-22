#ifndef OVERLAY_CFILE_H
#define OVERLAY_CFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "bcm_host.h"
#include "vgfont.h"

#include "revision.h"

void updateOverlayTest(int iteration, int pow, int cad, int hr);
void updateOverlay(float spe, float dist, int pow, int cad, int hr, float temper, float humid, int batt);
void renderBatteryPercentage(int battPer);
void renderSpeed(float speed);
void renderPerfPercentage(float perfPer);
void renderCadence(int cadence);
void renderPower(int power);
void renderHR(int heartRate);
void renderTemp(float temperature);
void renderHumidity(float humidity);
        

void startOverlay();

void renderText (char text[], int x, int y, int size, char foreground[], char background[]);

void renderTextAligned(char text[], int x, int y, int size, char foreground[], char background[], char horizontal, char vertical);

void closeOverlay();


///////////////////////////////////////////////////////
// Constants and variables



#endif
