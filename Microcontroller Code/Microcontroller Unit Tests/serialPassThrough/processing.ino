

void processData (const char line) {
  // Similar to the fuction on the bike but altered to suit the pass through nature of this program

  /* Gets in a value for which line to process, then processes it.
     Requests characters are lowercase, setting is uppercase
     a - Heart rate (front) (BPM)
     b - Heart rate (rear) (BPM)
     c - Cadence (front) (RPM)
     d - Cadence (rear) (RPM)
     e - Power (front) (W)
     f - Power (rear) (W)
     i - Front battery %
     j - Rear battery %
     k - Backup system battery %
     h - Humidity (R.H.%)
     t - Temperature (C * 2 + 50)
     s - Speed (km/h)
     q - Distance (number of rotations)
     l - Latitude (degrees)
     m - Longitude (degrees)
     n - Altitude (m)
     o - GPS speed (km/h)
     p - GPS distance (km)
     u - Starting longitude
     v - Starting latitude
     y - Testing byte
     z - Testing byte
  */
  char frameType = 0; // character to store data frame type

  // Gets frame type from specific line
  switch (line) {
    case 'h':
      frameType = Serial.read();
      break;
    case 'u':
      frameType = Serial.read();
      break;

    // Telemetry
    case 't':
      frameType = radioMessage[0];
      break;
  }

  if (debugMode) {
    Serial.print(F("Frame type : "));
    Serial.print(frameType);
    Serial.print(F(" on line "));
    Serial.println(line);
  }

  // Process data depending on type
  String returnMessage = ""; // Stores return message
  bool request; // Used to store if the message is a request (need a respose sent back on the same line)

  if ((frameType >= 'a') && (frameType <= '}')) {

    // Request type
    // The data will be stored to "returnMessage"
    request = true;
    if (line == 't') {
      // Radio to serial
      Serial.print(radioMessage); // Send the request to the serial line
  
      unsigned long timeoutMark = millis() + requestTimeout;
  
      while (!Serial.available() && (timeoutMark > millis())) {
        // Kill time until either the timeout is reached or a meesage is recieved
        delayMicroseconds(100);
      }
  
      // Read in the data
      if (Serial.available()) {
        byte messageLength = byte(Serial.read()); // Get length
        returnMessage = char(messageLength); // Store length to pass on
        messageLength -= 31;
  
        char temp[messageLength];
        Serial.readBytes(temp, messageLength);
        returnMessage = returnMessage + temp; // Append data to length char for return
      }
    }
    else {
      // Serial to telemetry
      unsigned int timeoutMark;

      // Need to send request twice since the result will be in the acknoledge packet that follows
      // a request, so the first one is sent to prepare a response. The second request collects it.
      radioSend(String(frameType));
      
      radioSend(String(frameType));

      // Read in the data
      if (recievedRadioData) {
        recievedRadioData = false; // Reset flag

        if (radioMessage[0] == '[') {
          // Bulk transfer

          memcpy(&dataLoad, radioMessage, sizeof(dataLoad));

          if (debugMode) {
            Serial.println("");
            Serial.println(dataLoad.messageType);
            Serial.println(dataLoad.messageLength);
            Serial.println("Speeds:");
            Serial.println(dataLoad.speedEncoder);
            Serial.println(dataLoad.speedGPS);
            Serial.println("Dist:");
            Serial.println(dataLoad.rotations);
            Serial.println(dataLoad.distGPS);
            Serial.println("Brake Temps:");
            Serial.println(dataLoad.frontBrakeT);
            Serial.println(dataLoad.rearBrakeT);
            Serial.println("Batteries:");
            Serial.println(dataLoad.fBatt);
            Serial.println(dataLoad.rBatt);
            Serial.println("Atmosphere:");
            Serial.println(dataLoad.humid);
            Serial.println(dataLoad.temp);
            Serial.println(dataLoad.CO2);
            Serial.println("ANT:");
            Serial.println(dataLoad.fhr);
            Serial.println(dataLoad.rhr);
            Serial.println(dataLoad.fcad);
            Serial.println(dataLoad.rcad);
            Serial.println(dataLoad.fpwr);
            Serial.println(dataLoad.rpwr);
          }

          char * messingAround = (char *)&dataLoad;
          for (byte i = 0; i < sizeof(dataLoad); i++) {
              Serial.write(messingAround[i]);
          }
          return;
        }

        
        
        // Not the radio message will be prefixed with the length character
        returnMessage = radioMessage;
        returnMessage.remove(0, 2); // Remove type and length char
      }
    }
  } 
  else if ((frameType >= 'A') && (frameType <= ']')) {
    // Sending data
    // Send the type char along with the data associated
    request = false;
  
    // Get entire data frame
    String temp = readInput(line);
    char lengthChar = char(temp.length() + 31);
    temp = lengthChar + temp;
    temp = frameType + temp;
  
    if (line == 't') {
      // Radio to serial
      Serial.print(temp);
    }
    else {
      // Serial to radio
      radioSend(temp);
    }
  }

  // Sends return if request
  if (request) {
    if (returnMessage.length() == 0) return;
    sendMessage(frameType - 32, returnMessage, line);
  }
  if (debugMode) Serial.println("DONE PROCESSING");
}

void sendMessage (char messageType, String message, const char outputLine) {
  // Gets length byte and puts it at the front of message
  char lengthChar = message.length() + 31;

  // Sends response to appropriate line
  switch (outputLine) {
    case 'h':
      Serial.print(lengthChar);
      Serial.print(message);
      break;

    // Telemetry
    case 't':
      radioSend(String(lengthChar) + message);
      break;
  }

  if (debugMode) {
    Serial.print(F("Sent : "));
    Serial.print(message.length());
    Serial.print(F(" - "));
    Serial.print(message);
    Serial.print(F(" on line "));
    Serial.println(outputLine);
  }
}

String readInput (char inputLine) {
  int messageLength = 0; // Stores length
  char temp[50];          // Input buffer

  String message = "";

  // Goes to the right serial line, reads message length, then the message itself
  switch (inputLine) {
    case 'h':
      while (Serial.available() == 0) {
        delayMicroseconds(10); // Wait for length character if needed
      }
      messageLength = byte(Serial.read());
      messageLength -= 31;

      Serial.readBytes(temp, messageLength);
      break;
    case 'u':
      while (Serial.available() == 0) {
        delayMicroseconds(10); // Wait for length character if needed
      }
      messageLength = Serial.read();
      messageLength -= 31;

      Serial.readBytes(temp, messageLength);
      break;

    // Telemetry
    case 't':
      messageLength = byte(radioMessage[1]);
      messageLength -= 31;

      for (byte i = 0; i < messageLength; i++) {
        temp[i] = radioMessage[i + 2];
      }
      break;
  }

  // Copy message to string and trim the useless end
  message = String(temp);
  message.remove(messageLength, 50 - messageLength);

  if (debugMode) {
    Serial.print(F("Recieved: "));
    Serial.print(messageLength);
    Serial.print(F(" - "));
    Serial.print(message);
    Serial.print(F(" on line "));
    Serial.println(inputLine);
  }

  return (message);
}
