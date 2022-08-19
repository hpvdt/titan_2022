#include <Arduino.h>
#include "communication.h"
#include "sensors.h"

void setup() {

  setupCommunication(); // Must be first for potential begging
  setupSensors();
  

#ifdef ALLOW_DEBUG_SERIAL
    // Debug timeout
  const int timeout = 2000; // Time out for debugger to be started if debugging
  unsigned long endTime = millis() + timeout; // End time for scanning

  if (debugMode) {
    // Set up debugging mode (USB connection)
    DEBUGSERIAL.begin(debugBaud);

    while (!DEBUGSERIAL) {
      delay(100); // wait for serial port to connect. Needed for native USB

      if (millis() > endTime) {
        // Once timeout has been reached
        debugMode = false; // Disable debugging
        break; // Exit loop
      }
    }

    if (debugMode) {
      DEBUGSERIAL.println(F("DEBUGGING MODE")); // Print debug statement if no timeout
    }
  }
#endif

  // Radio Setup
  radioSetup();
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

  checkHDT();

  // Brake Temperature Check
  checkBrakeTemps();

  // Periodic battery check
  checkBatteries();

  // Periodic speed check, to see if we stopped
  checkEncoderTimeout();

  /////////////////////////////////////////
  // GPS code
  GPSCheck();

  delayMicroseconds(500); // Regular delay
}