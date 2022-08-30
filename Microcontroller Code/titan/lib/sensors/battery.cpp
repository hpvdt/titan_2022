#include "battery.h"

int FBatt = 100;            // Front battery %
int RBatt = 100;            // Rear battery %

const byte FBPin = PA0;   // Front battery pin (ADC)
const byte RBPin = PA1;   // Rear battery pin (ADC)

const unsigned long batteryPeriod = 2000; // Battery check period (every few seconds)

// Factor by which to divide ADC reading (4095 at max, 3.3 ref)
// Also need to voltage account for voltage division
// Also need record each STM32's '3.3V' rail actual voltage
const float readingToV = 1241.2121; // Given by 4096 over reference voltage (3.3V)

// Resistor voltage division factor
const float fFactor = 6.24880; // 6.24880 for board 1, 6.23402 for board 2
const float rFactor = 6.24240; // 6.24240 for board 1, 6.22913 for board 2

/* Convert voltage to % according to battery properties

    Uses linear interpolation to determine battery percentage.
    Starts from second highest set point (99%) and sees if the
    reading is higher than the set point. If it is, it'll
    interpolate using the set point and the next higher one.
    If the reading is less than the set point move to the next
    interval and repeat

    Voltages and percentages were sourced from:
    https://www.solacity.com/how-to-keep-lifepo4-lithium-ion-batteries-happy/
    And adjusted from 4 cells to 3 (by multiplying voltages by 0.75)
*/
const byte level[] = {100, 99, 90, 70, 40, 30, 20, 17, 14, 9, 0}; // Percentages linked to voltages
const float voltage[] = {10.2, 10.05, 9.975, 9.9, 9.825, 9.75, 9.675, 9.6, 9.375, 9, 7.5}; // Voltages

void setupBattery() {
  pinMode(FBPin, INPUT_ANALOG); // Battery pins
  pinMode(RBPin, INPUT_ANALOG);
}


byte batteryLevel (char line) {

  // Get the voltage on whichever line
  float reading = 0.0;
  switch (line) {
    case 'f':
      reading = float(analogRead(FBPin));
      reading /= readingToV;
      reading *= fFactor;
      break;
    case 'r':
      reading = float(analogRead(FBPin));
      reading /= readingToV;
      reading *= rFactor;
      break;
  }


  if (reading > 0) {
    

    // Run though set point from
    for (byte i = 1; i <= 11; i++) {
      if (reading > voltage[i]) {
        // If the reading is in the region
        float temp = 0; // Used in calculations

        // Linear interpolation formula
        temp = (reading - voltage[i]) * float(level[i - 1] - level[i]);
        temp /= float(voltage[i - 1] - voltage[i]);
        temp += level[i];

        reading = temp; // Stores the result in the reading variable
        break;
      }
    }
  }
  reading = constrain(reading, 0, 100); // Constrain it to reasonable values
  
  return (int(reading));
}

void checkBatteries() {
  static unsigned long batteryTime = 0;  // Stores time for next battery check

  if (millis() > batteryTime) {
    FBatt = batteryLevel('f');
    RBatt = batteryLevel('r');

    batteryTime = millis() + batteryPeriod; // Sets next check
  }
}
