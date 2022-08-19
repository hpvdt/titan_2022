#include <Arduino.h>
#include "communication.h"
#include "sensors.h"

void setup() {
  
  setupCommunication(); // Must be first for potential begging
  setupSensors();

}

void loop() {
  if (frontSerial.available()) {
    // Data on front line
    processData('f');
  }
  if (rearSerial.available()) {
    // Data on rear line
    processData('r');
  }
#ifdef ALLOW_DEBUG_SERIAL
  if (DEBUGSERIAL.available()) {
    // Data on debugging line
    processData('d');
  }
#endif

  // Telemetry Check
  if(recievedRadioData) {
    radioRecieved();
    recievedRadioData = false;
    processData('t');
#ifdef ALLOW_DEBUG_SERIAL
  if (debugMode) {
    DEBUGSERIAL.println("Sent back message on telemetry");
  }
#endif
  }

  // Periodic checks of different sensors
  // Their update conditions/periods are set in their files
  checkHDT();
  checkBrakeTemps();
  checkBatteries();
  checkEncoderTimeout();
  GPSCheck();

  delayMicroseconds(500); // Regular delay
}