#include "co2.h"
#include "led.h"

volatile int CO2ppm = 0; 

const byte CO2Pin = PB8;
const int CO2Span = 5000;  // Span in ppm of CO2 of sensor

void CO2change() {
  const bool currentState = digitalRead(CO2Pin);
  const unsigned long currentTime = millis();

  // Used to record previous edge times
  static unsigned long lastCO2Rising = 0;
  static unsigned long lastCO2Falling = 0;

  // Determine edge type based on current state
  if (currentState == HIGH) {
    // Treat it as a rising edge

    // Calculate CO2 level from last pulse
    // CO2 = ppm span * (Th - 2ms) / (Th + Tl - 4ms)

    unsigned long timeHigh = lastCO2Falling - lastCO2Rising;
    unsigned long timeLow = currentTime - lastCO2Rising;

    CO2ppm = CO2Span * (timeHigh - 2);
    CO2ppm = CO2ppm / (timeHigh + timeLow - 4);

    lastCO2Rising = currentTime; // Update rising time
  }
  else {
    // Falling edge
    lastCO2Falling = millis();
  }
}

void setupCO2() {
  pinMode(CO2Pin, INPUT);

  attachInterrupt(CO2Pin, CO2change, CHANGE);
}
