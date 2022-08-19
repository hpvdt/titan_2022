#ifndef TITAN_TELEMETRY_HEADER
#define TITAN_TELEMETRY_HEADER

#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"
#include "communication.h"

extern RF24 radio; // CE, CS
extern volatile bool recievedRadioData;           // Used to flag if an interupt flag was thrown
extern char radioMessage[32]; // Buffer for message (nRF24 messages are limited to 32 bytes each)

extern void radioSetup();
extern void checkRadio();
extern void radioRecieved();
extern void radioSend(String outputMessage);

#endif
