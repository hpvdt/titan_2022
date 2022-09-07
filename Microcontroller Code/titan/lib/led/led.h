#ifndef TITAN_LED_HEADER
#define TITAN_LED_HEADER

#include <Arduino.h>

void setupLEDs();
void ledOn(byte index); // Turn on a given status LED
void ledOff(byte index); // Turn off a given status LED
void ledSet(byte index, bool state); // Set a given status LED
void ledInvert(byte index); // Invert the state of a given status LED

/** @name blinkLEDBlocking
   *  @brief Blink an LED a given number of times, blocking other code until done
   *  @param index Status LED number to blink
   *  @param period The period (in ms) of the blinking
   *  @param times The number of blinks to exectute
   *  @note Will block code (prevent execution until complete)
   */
void blinkLEDBlocking(byte index, int period, int times);

/** @name heartBeat
   *  @brief Slowly changes the brightness of the heart beat LED with each few calls
   *  @note Used to help show processor load or if it freezes up
   */
void heartBeat();


#endif
