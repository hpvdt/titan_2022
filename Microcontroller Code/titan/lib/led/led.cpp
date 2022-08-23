#include "led.h"

const byte ledCount = 3;
const byte ledPins[] = {PA8, PB15, PB14};

void setupLEDs() {
    for (byte i = 0; i < ledCount; i++) {
        pinMode(ledPins[i], OUTPUT);
        // Cycle through LEDs to ensure they work
        digitalWrite(ledPins[i], HIGH);
        delay(500);
        digitalWrite(ledPins[i], LOW);
    }

}

void ledOn(byte index) {
    if (index > ledCount) return; // Too big
    digitalWrite(ledPins[index - 1], HIGH);
}

void ledOff(byte index) {
    if (index > ledCount) return; // Too big
    digitalWrite(ledPins[index - 1], LOW);
}

void invertLED(byte index) {
    if (index > ledCount) return; // Too big

    // Read and invert
    digitalWrite(ledPins[index - 1], !digitalRead(ledPins[index - 1]));
}

void blinkLEDBlocking(byte index, int period, int times) {
    if (index > ledCount) return; // Too big

    for (int i = 0; i < (times * 2); i++) {
        invertLED(index);
        delay(period / 2);
    }
}

void heartBeat() {
    // Slowly changes the brightness of the heart beat LED with each few calls
    // Used to detect hangups
    const int numberCallsperiod = 2;
    static int callsSinceLastChange = 0;
    static int increment = 1;
    static byte duty = 0;

    // Check if it is time to adjust duty
    if (callsSinceLastChange == numberCallsperiod) {
        callsSinceLastChange = 0;

        duty = duty + increment;
        analogWrite(ledPins[0], duty);

        // Count the other way at extremes
        if (duty == 0) increment = 1;
        if (duty == 255) increment = -1;
    }
    else callsSinceLastChange++;  
}
