#ifndef TITAN_ENCODER_HEADER
#define TITAN_ENCODER_HEADER

#include <Arduino.h>

extern volatile float speedEncoder; // Speed (km/h) from encoder
extern volatile unsigned int rotationCount; // Number of complete wheel roations


void setupEncoder();
void encoderDetect();
void checkEncoderTimeout();

#endif
