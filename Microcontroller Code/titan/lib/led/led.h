#ifndef TITAN_LED_HEADER
#define TITAN_LED_HEADER

#include <Arduino.h>

void setupLEDs();
void ledOn(byte index);
void ledOff(byte index);
void invertLED(byte index);
void blinkLEDBlocking(byte index, int period, int times);

/** @name heartBeat
   *  @brief Slowly changes the brightness of the heart beat LED with each few calls
   *  @note Used to help show processor load or if it freezes up
   */
void heartBeat();


#endif
