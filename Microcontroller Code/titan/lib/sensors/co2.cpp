#include "co2.h"

volatile int CO2ppm = 0; 

const byte CO2Pin = PB8;
volatile unsigned long lastCO2Falling = 0;
const int CO2Span = 5000;  // Span in ppm of CO2 of sensor

void CO2Rising() {
  static unsigned long lastCO2Rising = 0;
  // Rising edge, start of a new pulse

  // Calculate CO2 level from last pulse
  // CO2 = ppm span * (Th - 2ms) / (Th + Tl - 4ms)

  unsigned long timeHigh = lastCO2Rising - lastCO2Falling;
  unsigned long timeLow = millis() - lastCO2Rising;

  CO2ppm = CO2Span * (timeHigh - 2);
  CO2ppm = CO2ppm / (timeHigh + timeLow - 4);

  lastCO2Rising = millis() ; // Update rising time
}

void CO2Falling() {
  lastCO2Falling = millis();
}

void setupCO2() {
  pinMode(CO2Pin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(CO2Pin), CO2Rising, RISING);
  attachInterrupt(digitalPinToInterrupt(CO2Pin), CO2Falling, FALLING);
}
