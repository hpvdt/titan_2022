#include "encoder.h"

const byte encoderPin = PB9;   // Encoder interrupt pin

volatile float speedEncoder = 0.0; // Speed (km/h) from encoder
volatile unsigned int rotationCount = 0; // Number of complete wheel roations

const byte numberTicks = 6; // Number of ticks per complete rotation of wheel
volatile byte currentTick = 0; // Stores which tick we are on
volatile byte previousTick = 1; // Stores what tick we were previously on

const float periodToKMH = 7545600.0; //7495200.0; on TITAN // Divide this by period in microseconds to get speed in km/h
// periodToKMH is determined by multiplying circumference in meters by 3.6 x 10^6
volatile unsigned long lastPass[numberTicks]; // Stores the last trigger time for each tick
const unsigned long debounce = 4000; // Debounce period in us. Prevents there being repeated measurements from flickering

const unsigned int wheelTimeout = 5000; // Timeout period without rotation (ms)

void setupEncoder() {
  pinMode(encoderPin, INPUT_PULLUP); // Encoder interupt

  attachInterrupt(encoderPin, encoderDetect, FALLING); // Encoder interupt

  for (byte i = 0; i < numberTicks; i++) {
    lastPass[i] = 0; // Reset all encoder tick values
  }
}

void encoderDetect() {
  const unsigned long triggerTime = micros(); // Use micros instead of millis for higher precision

  if (triggerTime > (lastPass[previousTick] + debounce)) {
    // Checks if we have passed the debounce period from the last measurement

    speedEncoder = periodToKMH / (triggerTime - lastPass[currentTick]); // Gets speed from period
    
    lastPass[currentTick] = triggerTime; // Update trigger time for next run

    previousTick = currentTick; // Marks the current tick as the previous one for the next call
    // Using this variable reduces the calculations that occur when this routine is called

    // Updates tick indexes, cycling around when at the end
    currentTick++;
    if (currentTick == numberTicks) {
      currentTick = 0;
      rotationCount++; // Increases rotation count with each full cycle
    }
  }
}


void checkEncoderTimeout() {
  static unsigned long wheelTime = 0; // Mark previous time checked
  static float previousSpeed = 0; // Stores speed for comparison

  if (millis() > wheelTime) {
    if (speedEncoder == previousSpeed) speedEncoder = 0; // Reset speed if no change

    previousSpeed = speedEncoder; // Updates speed
    wheelTime = millis() + wheelTimeout; // Sets next check
  }
}