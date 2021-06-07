void processData (const char line) {
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
    case 'f':
      frameType = FRONTSERIAL.read();
      break;
    case 'r':
      frameType = REARSERIAL.read();
      break;
    case 'd':
      frameType = DEBUGSERIAL.read();
      break;
  }

  if (debugMode) {
    DEBUGSERIAL.print(F("Frame type : "));
    DEBUGSERIAL.print(frameType);
    DEBUGSERIAL.print(F(" on line "));
    DEBUGSERIAL.println(line);
  }

  // Process data depending on type
  String returnMessage = ""; // Stores return message

  switch (frameType) {
    case 'a': // Heart rates
      returnMessage = String(FHR);
      break;
    case 'A':
      FHR = (readInput(line)).toInt();
      break;
    case 'b':
      returnMessage = String(RHR);
      break;
    case 'B':
      RHR = (readInput(line)).toInt();
      break;

    case 'c': // Cadences
      returnMessage = String(FCadence);
      break;
    case 'C':
      FCadence = (readInput(line)).toInt();
      break;
    case 'd':
      returnMessage = String(RCadence);
      break;
    case 'D':
      RCadence = (readInput(line)).toInt();
      break;

    case 'e': // Powers
      returnMessage = String(FPower);
      break;
    case 'E':
      FPower = readInput(line).toInt();
      break;
    case 'f':
      returnMessage = String(RPower);
      break;
    case 'F':
      RPower = readInput(line).toInt();
      break;

    case 'i': // Batteries
      returnMessage = String(FBatt);
      break;
    case 'I':
      FBatt = (readInput(line)).toInt();
      break;
    case 'j':
      returnMessage = String(RBatt);
      break;
    case 'J':
      RBatt = (readInput(line)).toInt();
      break;
    case 'k':
      returnMessage = String(SBatt);
      break;
    case 'K':
      SBatt = (readInput(line)).toInt();
      break;

    case 'h': // Humidity
      returnMessage = char(humidity);
      break;
    case 'H':
      humidity = byte(readInput(line)[0]);
      break;
    case 't': // Temperature
      returnMessage = char(temperature);
      break;
    case 'T':
      temperature = byte(readInput(line)[0]);
      break;

    case 's': // Speed
      returnMessage = String(speedKm, 3); // Speed to 3 decimal places
      break;
    case 'S':
      speedKm = readInput(line).toFloat();
      break;
    case 'q': // Distance
      returnMessage = String(rotationCount); // Distance
      break;
    case 'Q':
      rotationCount = readInput(line).toInt();
      break;

    case 'l': // Latitude
      returnMessage = String(latitude, 4); // Latitude to 3 decimal places
      break;
    case 'L':
      latitude = readInput(line).toFloat();
      break;
    case 'm': // Longitude
      returnMessage = String(longitude, 4); // Longitude to 3 decimal places
      break;
    case 'M':
      longitude = readInput(line).toFloat();
      break;
    case 'n': // Altitude
      returnMessage = String(altitudeGPS, 1); // Altitude to 1 decimal place
      break;
    case 'N':
      altitudeGPS = readInput(line).toFloat();
      break;
    case 'o': // Altitude
      returnMessage = String(speedGPS, 1); // Altitude to 1 decimal place
      break;
    case 'O':
      speedGPS = readInput(line).toFloat();
      break;
    case 'p': // Distance by GPS
      returnMessage = String(distanceGPS); // Distance
      break;
    case 'P':
      distanceGPS = readInput(line).toFloat();
      break;
      
    case 'u': // Starting location
      returnMessage = String(startLong); // Distance
      break;
    case 'U':
      startLong = readInput(line).toFloat();
      break;
    case 'v':
      returnMessage = String(startLat); // Distance
      break;
    case 'V':
      startLat = readInput(line).toFloat();
      break;
      
    case 'y':
      returnMessage = char(testY); // Test return
      break;
    case 'Y':
      testY = byte(readInput(line)[0]); // Test input
      break;
    case 'z':
      returnMessage = char(testZ); // Test return
      break;
    case 'Z':
      testZ = byte(readInput(line)[0]); // Test input
      break;
  }

  // Sends message if required (non empty)
  if (returnMessage != "") {
    sendMessage(returnMessage, line);
  }
}

void sendMessage (String message, const char outputLine) {
  // Gets length byte and puts it at the front of message
  char lengthChar = message.length() + 31;

  // Sends response to appropriate line
  switch (outputLine) {
    case 'f':
      FRONTSERIAL.print(lengthChar);
      FRONTSERIAL.print(message);
      break;
    case 'r':
      REARSERIAL.print(lengthChar);
      REARSERIAL.print(message);
      break;
    case 'd':
      DEBUGSERIAL.print(lengthChar);
      DEBUGSERIAL.print(message);
      break;
  }

  if (debugMode) {
    DEBUGSERIAL.print(F("Sent : "));
    DEBUGSERIAL.print(message.length());
    DEBUGSERIAL.print(F(" - "));
    DEBUGSERIAL.print(message);
    DEBUGSERIAL.print(F(" on line "));
    DEBUGSERIAL.println(outputLine);
  }
}

String readInput (char inputLine) {
  int messageLength = 0; // Stores length
  char temp[50];          // Input buffer

  String message = "";

  // Goes to the right serial line, reads message length, then the message itself
  switch (inputLine) {
    case 'f':
      while (FRONTSERIAL.available() == 0) {
        delayMicroseconds(10); // Wait for length character if needed
      }
      messageLength = byte(FRONTSERIAL.read());
      messageLength -= 31;

      FRONTSERIAL.readBytes(temp, messageLength);
      break;
    case 'r':
      while (REARSERIAL.available() == 0) {
        delayMicroseconds(10); // Wait for length character if needed
      }
      messageLength = REARSERIAL.read();
      messageLength -= 31;

      REARSERIAL.readBytes(temp, messageLength);
      break;
    case 'd':
      while (DEBUGSERIAL.available() == 0) {
        delayMicroseconds(10); // Wait for length character if needed
      }
      messageLength = DEBUGSERIAL.read();
      messageLength -= 31;

      DEBUGSERIAL.readBytes(temp, messageLength);
      break;
  }

  // Copy message to string and trim the useless end
  message = String(temp);
  message.remove(messageLength, 50 - messageLength);

  if (debugMode) {
    DEBUGSERIAL.print(F("Recieved: "));
    DEBUGSERIAL.print(messageLength);
    DEBUGSERIAL.print(F(" - "));
    DEBUGSERIAL.print(message);
    DEBUGSERIAL.print(F(" on line "));
    DEBUGSERIAL.println(inputLine);
  }

  return (message);
}
