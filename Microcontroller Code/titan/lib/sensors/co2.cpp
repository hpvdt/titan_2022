#include "co2.h"
#include "led.h"

volatile int CO2ppm = 0; 

const byte CO2Pin = PB8;
volatile unsigned long lastCO2Falling = 0;
const int CO2Span = 5000;  // Span in ppm of CO2 of sensor

void CO2Rising() {
  static unsigned long lastCO2Rising = 0;
  // Rising edge, start of a new pulse

  unsigned long currentTime = millis();

  // Calculate CO2 level from last pulse
  // CO2 = ppm span * (Th - 2ms) / (Th + Tl - 4ms)

  unsigned long timeHigh = lastCO2Falling - lastCO2Rising;
  unsigned long timeLow = currentTime - lastCO2Rising;

  CO2ppm = CO2Span * (timeHigh - 2);
  CO2ppm = CO2ppm / (timeHigh + timeLow - 4);

  lastCO2Rising = currentTime; // Update rising time
}

void CO2Falling() {
  lastCO2Falling = millis();
}

void setupCO2() {
  pinMode(CO2Pin, INPUT);

  // Interrupts don't seem to be activating. No idea why.
  // Wait, can probably only have one interrupt!!! Need one for CHANGE
  attachInterrupt(CO2Pin, CO2Rising, RISING);
  attachInterrupt(CO2Pin, CO2Falling, FALLING);
}

void CO2Check() {
  static bool lastState = LOW;

  // Calculate CO2 on edges
  // Has to be done by polling, interrupts aren't working.
  if (digitalRead(CO2Pin) != lastState) {
    if (lastState == LOW) CO2Rising();
    else CO2Falling();

    lastState = digitalRead(CO2Pin); // Record for next edge
  }
}