#include "telemetry.h"

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
    return;
  }

  // Set up radio if present
  radio.setPALevel(RF24_PA_MIN);          // Low power, raise if a decoupling capacitor is added
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
  attachInterrupt(digitalPinToInterrupt(RFInterruptPin), checkRadio, FALLING); // Set flagging interrupt
}

void checkRadio() {
  recievedRadioData = true;
}

void radioRecieved() {
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

void radioSend(String outputMessage) {
  byte messageLen = outputMessage.length();

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
    radio.writeAckPayload(pipeNumber, outputMessage.c_str(), messageLen);
  }
  else {
    // Sender
    radio.write(&outputMessage, outputMessage.length());
  }
}