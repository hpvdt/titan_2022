void setupRadio() {
  
  // Start radio system
  radio.begin();

  // Check for chip
  if (radio.isChipConnected() == false) {
    if (debugMode == true) {
      Serial.println("!!! nRF24L01 NOT DETECTED !!!");
    }
    recievedRadioData = false; // Ensure this is false
    return;
  }

  // Set up the radio if present
  radio.setPALevel(RF24_PA_MIN);          // Low power, raise if a decoupling capacitor is added
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(1);                    // Ensure autoACK is disabled
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.setRetries(10, 15);                // Smallest time between retries, max no. of retries

  radio.enableDynamicPayloads();
  radio.setChannel(100);

  if (recieverRole == true) {
    // Reciever
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1, pipes[0]);
    radio.startListening();
  }
  else {
    // Sender
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1, pipes[1]);
    radio.stopListening();
  }
  radio.maskIRQ(1, 1, 0);                 // Only toggles interrupt (falling edge/LOW) when recieving data
  //radio.printDetails();
  
  // Interrupts on recieving a message
  pinMode(radioInteruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(radioInteruptPin), radioRecieved, FALLING); // Set flagging interrupt
}

void checkRadio () {
  recievedRadioData = true; // Set flag that something was recieved
}

void radioRecieved() {
  recievedRadioData = true;
  memset(radioMessage, 0, sizeof(radioMessage)); // Clear buffer
  
  // Start by getting where the message is from
  radio.available(&pipeNumber); // Get pipe the message we recieved on in the event a reply is needed

  // Read the message. Reads both messages and acknoledgements. 
  int messageLen = radio.getDynamicPayloadSize();
  radio.read(&radioMessage, messageLen);

  if (debugMode) {
    Serial.print("Recieved on nRF24: '");
    Serial.print(radioMessage);
    Serial.print("'. Length of ");
    Serial.println(messageLen);
  }
}

void radioSend(String outputMessage) {
  int mesLength = outputMessage.length();

   if (debugMode) {
    Serial.print("Sending to nRF24: '");
    Serial.print(outputMessage);
    Serial.print("', length of ");
    Serial.println(mesLength);
  }
  
  if (recieverRole == true) {
    // Reciever
    // Uses the pointer to the text as a character array
    radio.writeAckPayload(pipeNumber, outputMessage.c_str(), mesLength);
  }
  else {
    // Sender, request an acknoledgement
    radio.write((outputMessage.c_str()), mesLength);
  }

}
