#ifndef TITAN_COMMUNICATION_HEADER
#define TITAN_COMMUNICATION_HEADER

#include <Arduino.h>
#include "sensors.h"
#include "telemetry.h"

// Ensure that 'Serial' is connected to USB if USB is enabled.
#if defined (USBCON) && !(defined(USBD_USE_CDC))
#error "Generic CDC is not superceding UART for 'Serial'. Check and correct USB build settings." 
#endif

// If build options enable USB and it is main 'Serial', automatically allow debugging
#if defined (USBCON) && defined(USBD_USE_CDC)
#define ALLOW_DEBUG_SERIAL // Enables debug code to be included in compilation
#define DEBUGSERIAL Serial // USB Connection
extern const long debugBaud;  // Baudrate for debugging line
#endif

// DEBUGGING MODE
extern bool debugMode;      // In debug mode or not (copies all messages to debug serial line

extern const long rpiBaud;  // Baudrate used with rpis
extern const long gpsBaud;    // Baudrate used with GPS

extern HardwareSerial frontSerial; //RX, TX
extern HardwareSerial rearSerial;
extern HardwareSerial gpsSerial;

// Communication specific variables
extern byte testY;             // Used solely for echo tests
extern byte testZ;             // Used solely for echo tests

// ANT Variables (since they only pass through here)
extern int FHR;               // Front heart rate (BPM)
extern int RHR;               // Rear heart rate
extern int FCadence;          // Cadence (RPM)
extern int RCadence;
extern int FPower;            // Power (W)
extern int RPower;

// Communication functions
void setupCommunication();
void processData (const char line);
void sendMessage (char messageType, String message, const char outputLine);
String readInput (char inputLine);

#endif
