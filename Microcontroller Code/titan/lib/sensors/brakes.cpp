#include "brakes.h"

float frontBrakeTemp = 0;   // Front brake temperature
float rearBrakeTemp = 0;    // Rear brake temperature

Adafruit_MLX90614 frontBrake = Adafruit_MLX90614();
Adafruit_MLX90614 rearBrake = Adafruit_MLX90614();

const byte frontBrakeAdd = 0x01;
const byte rearBrakeAdd = 0x02;
const float frontEmissivity = 1.0;
const float rearEmissivity = 1.0;
const unsigned long brakePeriod = 1000;


void setupBrakeThermometers() {
  if (frontBrake.begin(frontBrakeAdd)) {
#ifdef ALLOW_DEBUG_SERIAL
    if (debugMode) {
      DEBUGSERIAL.print("Error connecting to FRONT brake sensor at address: ");
      DEBUGSERIAL.println(frontBrakeAdd);
    }
#endif
  }
  if (rearBrake.begin(rearBrakeAdd)) {
#ifdef ALLOW_DEBUG_SERIAL
    if (debugMode) {
      DEBUGSERIAL.print("Error connecting to REAR brake sensor at address: ");
      DEBUGSERIAL.println(rearBrakeAdd);
    }
#endif
  }

  frontBrake.writeEmissivity(frontEmissivity);
  rearBrake.writeEmissivity(rearEmissivity);
}

void getBrakeTemps() {
  frontBrakeTemp = frontBrake.readObjectTempC();
  rearBrakeTemp = rearBrake.readObjectTempC();
}

void checkBrakeTemps() {
  static unsigned long brakeTime = 0;
  
  if (millis() > brakeTime) {
    getBrakeTemps();
    
    brakeTime = millis() + brakePeriod;
  }
}
