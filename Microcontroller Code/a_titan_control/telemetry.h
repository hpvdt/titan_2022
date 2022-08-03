#ifndef TITAN_TELEMETRY_HEADER
#define TITAN_TELEMETRY_HEADER
#include "RF24.h"

RF24 radio(PB1, PB0); // CE, CS
const byte RFInterruptPin = PA4; // IRQ pin

// Topology
const uint64_t pipes[2] = { 0x1122334471LL, 0x112233447CLL };  // Radio pipe addresses for the 2 nodes to communicate.

const bool recieverRole = true;                  // The role of the current running sketch
volatile bool recievedMessage = false;           // Used to flag if an interupt flag was thrown

char radioMessage[32]; // Buffer for message (nRF24 messages are limited to 32 bytes each)
byte pipeNumber; // Stores the origin of the message (used in reciever mode)


#endif