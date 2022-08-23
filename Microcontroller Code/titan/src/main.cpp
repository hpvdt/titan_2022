#include <Arduino.h>
#include "communication.h"
#include "sensors.h"
#include "led.h"

void setup() {

  setupCommunication(); // Must be first for potential begging
  setupSensors();

  setupLEDs();
  ledOn(2); // Turn on LED 2 for all operation
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
  }

  // Periodic checks of different sensors
  // Their update conditions/periods are set in their files
  checkHDT();
  checkBrakeTemps();
  checkBatteries();
  checkEncoderTimeout();
  GPSCheck();

  heartBeat(); // Used to visualize load
  delayMicroseconds(500); // Regular delay
}
