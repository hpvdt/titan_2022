#include "communication.h"

// If build options enable USB and it is main 'Serial', automatically allow debugging
#if defined (USBCON) && defined(USBD_USE_CDC)
#define ALLOW_DEBUG_SERIAL // Enables debug code to be included in compilation
#define DEBUGSERIAL Serial // USB Connection
const long debugBaud = 38400;  // Baudrate for debugging line
#endif

// DEBUGGING MODE
bool debugMode = true;      // In debug mode or not (copies all messages to debug serial line

const long rpiBaud = 115200;  // Baudrate used with rpis
const long gpsBaud = 9600;    // Baudrate used with GPS

HardwareSerial frontSerial(PA10, PA9); //RX, TX
HardwareSerial gpsSerial(PA3, PA2);
HardwareSerial rearSerial(PB11, PB10);

// Communication specific variables
byte testY = 0;             // Used solely for echo tests
byte testZ = 0;             // Used solely for echo tests

// ANT Variables (since they only pass through here)
int FHR = 10;               // Front heart rate (BPM)
int RHR = 10;               // Rear heart rate
int FCadence = 10;          // Cadence (RPM)
int RCadence = 10;
int FPower = 10;            // Power (W)
int RPower = 10;

struct bulkDataStruct {
  char messageType;
  char messageLength;
  uint16_t distGPS;
  uint32_t speedEncoder;
  uint32_t speedGPS;
  uint16_t rotations;
  uint16_t frontBrakeT;
  uint16_t rearBrakeT;
  uint8_t fBatt;
  uint8_t rBatt;
  uint8_t humid;
  uint8_t temp;
  uint16_t CO2;
  uint8_t fhr;
  uint8_t rhr;
  uint8_t fcad;
  uint8_t rcad;
  uint16_t fpwr;
  uint16_t rpwr;
};


void setupCommunication() {
  // Start each serial line
  frontSerial.begin(rpiBaud);
  rearSerial.begin(rpiBaud);
  gpsSerial.begin(gpsBaud);

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

void processData (const char line) {
  /* Gets in a value for which line to process, then processes it.
  Requests characters are lowercase, setting is uppercase

  a - Heart rate (front) (BPM)
  b - Heart rate (rear) (BPM)
  c - Cadence (front) (RPM)
  d - Cadence (rear) (RPM)
  e - Power (front) (W)
  f - Power (rear) (W)
  g - All ANT data (delimited, in order a-f)

  i - Front battery %
  j - Rear battery %

  h - Humidity (R.H.%)
  t - Temperature (C * 2 + 50)
  k - CO2 (ppm)

  s - Speed (km/h)
  q - Distance (number of rotations)

  l - Latitude (degrees)
  m - Longitude (degrees)
  n - Altitude (m)
  o - GPS speed (km/h)
  p - GPS distance (km)
  u - Starting longitude
  v - Starting latitude

  w - Front brake temperature (C)
  x - Rear brake temperature (C)

  y - Testing byte
  z - Testing byte
  */
  char frameType = 0; // character to store data frame type

  // Gets frame type from specific line
  switch (line) {
    case 'f':
      frameType = frontSerial.read();
      break;
    case 'r':
      frameType = rearSerial.read();
      break;
    
    // Telemetry
    case 't':
      frameType = radioMessage[0];
      break;
      
#ifdef ALLOW_DEBUG_SERIAL
    case 'd':
      frameType = DEBUGSERIAL.read();
      break;
#endif
  }

#ifdef ALLOW_DEBUG_SERIAL
  if (debugMode) {
    DEBUGSERIAL.print(F("Frame type : "));
    DEBUGSERIAL.print(frameType);
    DEBUGSERIAL.print(F(" on line "));
    DEBUGSERIAL.println(line);
  }
#endif

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
    case 'g':
      returnMessage = String(String(FHR) + "," + String(RHR) 
                            + "," + String(FCadence) + "," + String(RCadence) 
                            + "," + String(FPower) + "," + String(RPower));
      break;
    case 'G':
      returnMessage = readInput(line); // Use return string temporarily

      for (byte i = 0; i < 6; i++) {
        int temporaryANT;
        temporaryANT = 100 * (returnMessage[i * 4] - '0');
        temporaryANT += 10 * (returnMessage[i * 4 + 1] - '0');
        temporaryANT += returnMessage[i * 4 + 2] - '0';
        
        if (i == 0) FHR = temporaryANT;
        else if (i == 1) RHR = temporaryANT;
        else if (i == 2) FCadence = temporaryANT;
        else if (i == 3) RCadence = temporaryANT;
        else if (i == 4) FPower = temporaryANT;
        else if (i == 5) RPower = temporaryANT;
      }
      
      returnMessage = ""; //Clear it to not send it back
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
      
    case 'k': // CO2
      returnMessage = String(CO2ppm); // Distance
      break;
    case 'K':
      CO2ppm = readInput(line).toInt();
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
      returnMessage = String(speedEncoder, 3); // Speed to 3 decimal places
      break;
    case 'S':
      speedEncoder = readInput(line).toFloat();
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
      returnMessage = String(startLong);
      break;
    case 'U':
      startLong = readInput(line).toFloat();
      break;
    case 'v':
      returnMessage = String(startLat); 
      break;
    case 'V':
      startLat = readInput(line).toFloat();
      break;
      
    case 'w': // Front Brake Temperature
      returnMessage = String(frontBrakeTemp, 4); // Latitude to 3 decimal places
      break;
    case 'W':
      frontBrakeTemp = readInput(line).toFloat();
      break;
    case 'x': // Rear Brake Temperature
      returnMessage = String(rearBrakeTemp, 4); // Longitude to 3 decimal places
      break;
    case 'X':
      rearBrakeTemp = readInput(line).toFloat();
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

    case '{':
      bulkDataStruct dataLoad;
      dataLoad.messageType = '[';
      dataLoad.messageLength = sizeof(dataLoad) - 2 + 31;
      dataLoad.distGPS = distanceGPS * 1000;
      dataLoad.speedEncoder = 1000 * speedEncoder;
      dataLoad.speedGPS = 1000 * speedGPS;
      dataLoad.rotations = rotationCount;
      dataLoad.frontBrakeT = frontBrakeTemp * 100;
      dataLoad.rearBrakeT = rearBrakeTemp * 100;
      dataLoad.fBatt = FBatt;
      dataLoad.rBatt = RBatt;
      dataLoad.humid = humidity;
      dataLoad.temp = temperature;
      dataLoad.CO2 = CO2ppm;
      dataLoad.fhr = FHR;
      dataLoad.rhr = RHR;
      dataLoad.fcad = FCadence;
      dataLoad.rcad = RCadence;
      dataLoad.fpwr = FPower;
      dataLoad.rpwr = RPower;

      sendMessageArbitrary((byte *) &dataLoad, sizeof(dataLoad), line);
      break;
  }

  // Sends message if required (non empty)
  if (returnMessage != "") {
    sendMessageString(returnMessage, line);
  }
}

void sendMessageString (String message, const char outputLine) {
  // Gets length byte and puts it at the front of message
  char lengthChar = message.length() + 31;

  // Sends response to appropriate line
  switch (outputLine) {
    case 'f':
      frontSerial.print(lengthChar);
      frontSerial.print(message);
      break;
    case 'r':
      rearSerial.print(lengthChar);
      rearSerial.print(message);
      break;

    // Telemetry
    case 't':
      radioSend(String(String(lengthChar) + message).c_str(), lengthChar - 30);
      break;
#ifdef ALLOW_DEBUG_SERIAL
    case 'd':
      DEBUGSERIAL.print(lengthChar);
      DEBUGSERIAL.print(message);
      break;
#endif
  }

#ifdef ALLOW_DEBUG_SERIAL
  if (debugMode) {
    DEBUGSERIAL.print(F("Sent : "));
    DEBUGSERIAL.print(message.length());
    DEBUGSERIAL.print(F(" - "));
    DEBUGSERIAL.print(message);
    DEBUGSERIAL.print(F(" on line "));
    DEBUGSERIAL.println(outputLine);
  }
#endif
}


void sendMessageArbitrary (byte* message, const byte payloadLength, const char outputLine) {
  
  // Sends response to appropriate line
  switch (outputLine) {
    case 'f':
      for (byte i = 0; i < payloadLength; i++) frontSerial.write(message[i]);
      break;
    case 'r':
      for (byte i = 0; i < payloadLength; i++) rearSerial.write(message[i]);
      break;

    // Telemetry
    case 't':
      radioSend((const char *) message, payloadLength);
      break;
#ifdef ALLOW_DEBUG_SERIAL
    case 'd':
      for (byte i = 0; i < payloadLength; i++) DEBUGSERIAL.write(message[i]);
      break;
#endif
  }

#ifdef ALLOW_DEBUG_SERIAL
  if (debugMode) {
    DEBUGSERIAL.print(F("Sent : "));
    DEBUGSERIAL.print(message.length());
    DEBUGSERIAL.print(F(" - "));
    DEBUGSERIAL.print(message);
    DEBUGSERIAL.print(F(" on line "));
    DEBUGSERIAL.println(outputLine);
  }
#endif

}

String readInput (char inputLine) {
  int messageLength = 0; // Stores length
  char temp[50];          // Input buffer

  String message = "";

  // Goes to the right serial line, reads message length, then the message itself
  switch (inputLine) {
    case 'f':
      while (frontSerial.available() == 0) {
        delayMicroseconds(10); // Wait for length character if needed
      }
      messageLength = byte(frontSerial.read());
      messageLength -= 31;

      frontSerial.readBytes(temp, messageLength);
      break;
    case 'r':
      while (rearSerial.available() == 0) {
        delayMicroseconds(10); // Wait for length character if needed
      }
      messageLength = rearSerial.read();
      messageLength -= 31;

      rearSerial.readBytes(temp, messageLength);
      break;

    // Telemetry
    case 't':
      messageLength = byte(radioMessage[1]);
      messageLength -= 31;

      for (byte i = 0; i < messageLength; i++) {
        temp[i] = radioMessage[i + 2];
      }
      break;
#ifdef ALLOW_DEBUG_SERIAL
    case 'd':
      while (DEBUGSERIAL.available() == 0) {
        delayMicroseconds(10); // Wait for length character if needed
      }
      messageLength = DEBUGSERIAL.read();
      messageLength -= 31;

      DEBUGSERIAL.readBytes(temp, messageLength);
      break;
#endif
  }

  // Copy message to string and trim the useless end
  message = String(temp);
  message.remove(messageLength, 50 - messageLength);

#ifdef ALLOW_DEBUG_SERIAL
  if (debugMode) {
    DEBUGSERIAL.print(F("Recieved: "));
    DEBUGSERIAL.print(messageLength);
    DEBUGSERIAL.print(F(" - "));
    DEBUGSERIAL.print(message);
    DEBUGSERIAL.print(F(" on line "));
    DEBUGSERIAL.println(inputLine);
  }
#endif
  return (message);
}
