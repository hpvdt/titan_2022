#include "brakes.h"
#include <Adafruit_MLX90614.h>
#include "communication.h"
#include <Wire.h>
#include "led.h"

//            SDA  SCL
TwoWire brakeI2C(PB7, PB6);

float frontBrakeTemp = 0;   // Front brake temperature (deg C)
float rearBrakeTemp = 0;    // Rear brake temperature (deg C)

Adafruit_MLX90614 frontBrake = Adafruit_MLX90614();
Adafruit_MLX90614 rearBrake = Adafruit_MLX90614();

const byte frontBrakeAdd = 0x02;
const byte rearBrakeAdd = 0x01;
const float frontEmissivity = 0.6; // 0.6 for stainless steel used for brake disks
const float rearEmissivity = 0.6;
const unsigned long brakePeriod = 1000;

bool frontBrakeMissing, rearBrakeMissing;

void setupBrakeThermometers() {
  brakeI2C.begin();

  frontBrakeMissing = !frontBrake.begin(frontBrakeAdd, &brakeI2C);
  rearBrakeMissing = !rearBrake.begin(rearBrakeAdd, &brakeI2C);

  if (frontBrakeMissing) {
#ifdef ALLOW_DEBUG_SERIAL
    if (debugMode) {
      DEBUGSERIAL.print("Error connecting to FRONT brake sensor at address: ");
      DEBUGSERIAL.println(frontBrakeAdd);
    }
#endif

    // Light up LEDs to let it be known
    // LED 2 off as it blinks twice
    ledOff(1);
    ledOff(2);
    blinkLEDBlocking(3, 500, 2);
    ledOn(3); // Leave error on to inform users error has occured
    delay(2000);
  }
  else {
    // If connected
    frontBrake.writeEmissivity(frontEmissivity);
  }
  
  if (rearBrakeMissing) {
#ifdef ALLOW_DEBUG_SERIAL
    if (debugMode) {
      DEBUGSERIAL.print("Error connecting to REAR brake sensor at address: ");
      DEBUGSERIAL.println(rearBrakeAdd);
    }
#endif

    // Light up LEDs to let it be known
    // LED 2 on as it blinks twice
    ledOff(1);
    ledOn(2);
    blinkLEDBlocking(3, 500, 2);
    ledOn(3); // Leave error on to inform users error has occured
    delay(2000);
  }
  else {
    // If connected
    rearBrake.writeEmissivity(rearEmissivity);
  }
}

void getBrakeTemps() {
  // Only try to read sensors not present
  if (!frontBrakeMissing) frontBrakeTemp = frontBrake.readObjectTempC();
  if (!rearBrakeMissing) rearBrakeTemp = rearBrake.readObjectTempC();
}

void checkBrakeTemps() {
  static unsigned long brakeTime = 0;
  
  if (millis() > brakeTime) {
    getBrakeTemps();
    
    brakeTime = millis() + brakePeriod;
  }
}
