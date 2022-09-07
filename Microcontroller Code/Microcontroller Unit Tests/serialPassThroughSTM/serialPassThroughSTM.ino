// FOR ARDUINO NANO
#include <RF24.h>

const byte radioInteruptPin = PA4;
const byte radioCSPin = PB0;
const byte radioEnablePin = PB1;

// Hardware configuration
RF24 radio(radioEnablePin, radioCSPin);
bool recieverRole = false; // Are we the reciever? Vehicle is 'reciever', station is 'sender'

const int requestTimeout = 10;

// Communication pipes to use
const uint64_t pipes[2] = { 0x1122334471LL, 0x112233447CLL };

volatile bool recievedRadioData = false; // Used to record a recieve event
char radioMessage[32]; // Buffer for message (nRF24 messages are limited to 32 bytes each)
byte pipeNumber; // Stores the origin of the message (used in reciever mode)

unsigned long baudrate = 115200; // Baudrate on used on all serial lines
bool debugMode = false;





void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // Set up the serial lines
  Serial.begin(baudrate);
  
  if (debugMode == true) {
    // Wait for debugging connection, assumed to be over USB
    while (!Serial) {
      delay(100);
    }
    Serial.println("DEBUGGING MODE ACTIVE");
  }

  setupRadio();
  //blinker(1000, 3);

  if (debugMode == true) {
    Serial.println("GO TIME");
  }
}

void loop() {
  if (Serial.available()) {
    processData('h');
  }
  if (recievedRadioData) {
    processData('t');
    recievedRadioData = false; // Reset flag
  }
  delay(10);
}

void blinker (const int period, const byte repeats) {
  // Blink LED a number of times for a given length
  for (byte i = 0; i < repeats; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(period / 2);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(period / 2);
  }
  // Note on STM32 LED is on when pin is LOW!
}
