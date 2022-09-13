#ifndef TITAN_ENCODER_HEADER
#define TITAN_ENCODER_HEADER

#if !defined(STM32_CORE_VERSION) || (STM32_CORE_VERSION  < 0x01090000)
//#error "Due to API change, this sketch is compatible with STM32_CORE_VERSION  >= 0x01090000"
#endif

#include <Arduino.h>

extern volatile float speedEncoder; // Speed (km/h) from encoder
extern volatile unsigned int rotationCount; // Number of complete wheel roations


void setupEncoder();
void encoderDetect();
void checkEncoderTimeout();

#endif
