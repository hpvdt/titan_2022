#include "telemetry.h"
#include "led.h"

const byte RFCEpin = PB1;
const byte RFCSpin = PB0;
const byte RFInterruptPin = PA4; // IRQ pin

RF24 radio(RFCEpin, RFCSpin); // CE, CS
char radioMessage[32]; // Buffer for message (nRF24 messages are limited to 32 bytes each)

// Topology
const uint64_t pipes[2] = { 0x1122334471LL, 0x112233447CLL };  // Radio pipe addresses for the 2 nodes to communicate.
volatile bool recievedRadioData = false;           // Used to flag if an interupt flag was thrown
const bool recieverRole = true;                  // The role of the current running sketch
byte pipeNumber; // Stores the origin of the message (used in reciever mode)

void radioSetup() {

    // Start radio system
  radio.begin();

  // Check for chip, alert user and don't bother setting up if missing
  if (radio.isChipConnected() == false) {
#ifdef ALLOW_DEBUG_SERIAL 
    if (debugMode) {
      DEBUGSERIAL.println("!!! nRF24L01 NOT DETECTED !!!");
    }
#endif
    recievedRadioData = false; // Ensure this is false

    // LED alert
    ledOff(1);
    ledOff(2);
    blinkLEDBlocking(3, 500, 4);
    ledOn(3); // Leave error on to inform users error has occured
    delay(2000);

    return;
  }

  // Set up radio if present
  radio.setPALevel(RF24_PA_MAX);          // TODO: Verify power stability, lower if needed
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(1);                    // Ensure autoACK is disabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setRetries(5, 15);                // Smallest time between retries, max no. of retries
  radio.enableDynamicPayloads();
  radio.setChannel(100);
  if (recieverRole == true) {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1, pipes[0]);
    radio.startListening();
  }
  else {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1, pipes[1]);
    radio.stopListening();
  }
  //radio.printDetails();                   // Dump the configuration of the rf unit for debugging
  radio.maskIRQ(1, 1, 0);                 // Only toggles interrupt (falling edge/LOW) when recieving data

  // Interrupts on recieving a message
  pinMode(RFInterruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RFInterruptPin), radioRecieved, FALLING); // Set flagging interrupt
}

void radioRecieved() {
  recievedRadioData = true;

  // Start by getting where the message is from
  radio.available(&pipeNumber); // Get pipe the message was recieved on in the event a reply is needed

  // Read the message. Reads both messages and acknoledgements. 
  radio.read(&radioMessage, radio.getDynamicPayloadSize());

#ifdef ALLOW_DEBUG_SERIAL
  if (debugMode) {
    DEBUGSERIAL.print("Recieved on nRF24: ");
    DEBUGSERIAL.println(radioMessage);
  }
#endif
}

void radioSend(const char* outputMessage, byte messageLen) {

#ifdef ALLOW_DEBUG_SERIAL
  if (debugMode) {
    DEBUGSERIAL.print("Sent by nRF24: '");
    DEBUGSERIAL.print(outputMessage);
    DEBUGSERIAL.println("'");
  }
#endif
  
  if (recieverRole == true) {
    // Reciever
    // Uses the pointer to the text as a character array
    radio.writeAckPayload(pipeNumber, outputMessage, messageLen);
  }
  else {
    // Sender
    radio.write(&outputMessage, messageLen);
  }
}
