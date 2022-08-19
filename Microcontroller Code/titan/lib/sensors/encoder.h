#ifndef TITAN_ENCODER_HEADER
#define TITAN_ENCODER_HEADER

#include <Arduino.h>

extern volatile float speedKm; // Speed (km/h)
extern volatile unsigned int rotationCount; // Number of complete wheel roations


void setupEncoder();
void encoderDetect();
void checkEncoderTimeout();

#endif
