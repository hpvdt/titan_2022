#include "encodertimer.h"

/* Timer Based Encoder System

  Unlike the previous system which used an interrupt to record the period between spokes
  this version uses a hardware timer set up so that when an event ossurs the timer records 
  it right then and raises an interrupt flag.

  This means that the interrupt can be delayed (e.g. another interrupt is occuring) but
  the period will still be recorded accurately for the routine to then convert into a speed.
  It also enables interrupt/timer based timeouts for speed rather than depending on another
  piece of (non-interrupt) code.

  It will have issues with bouncing, so the input needs to be clean.
*/

const PinName encoderPin = PB_9;

volatile float speedEncoder = 0.0; // Speed (km/h) from encoder
volatile unsigned int rotationCount = 0; // Number of complete wheel roations

const byte numberTicks = 6; // Number of ticks per complete rotation of wheel
volatile byte currentTick = 0; // Stores which tick we are on

const float freqToKMH = 2.109 * 3.6; // Multiply rotation frequency by this to get speed in km/h

void checkEncoderTimeout() {
  // Empty, not needed for this, still defined so we can easily swap to old encoder system as needed
}

uint32_t channel;
uint32_t inputFreq = 0; // Stores reference frequency for counter
uint32_t channelOveflow; // Stores overflow value
volatile uint32_t rolloverCompareCount = 0; // Keeps track of successive overflows, reset on events
HardwareTimer *encoderTimer;

void inputCapture_IT_callback() {
  static uint32_t lastCapture = 0;

  uint32_t currentCapture = encoderTimer->getCaptureCompare(channel);

  // Event frequency computation
  float frequencyMeasured;
  if (currentCapture > lastCapture) {
    frequencyMeasured = float(inputFreq) / float(currentCapture - lastCapture);
  }
  else if (currentCapture <= lastCapture) {
    // Deal with overflow
    frequencyMeasured = float(inputFreq) / float((channelOveflow + currentCapture - lastCapture));
  }

  lastCapture = currentCapture;
  rolloverCompareCount = 0;

  // Convert to speed
  speedEncoder = frequencyMeasured * freqToKMH;

  // Count rotations
  currentTick++;
  if (currentTick == numberTicks) {
    currentTick = 0;
    rotationCount++;
  }
}

/* In case of multiple timer rollovers, speed is automatically reset */
void rollover_IT_callback() {
  rolloverCompareCount++;

  if (rolloverCompareCount > 1) speedEncoder = 0;
}

void setupEncoder() {
  pinMode(encoderPin, INPUT_PULLUP); // Encoder interupt

  // Automatically retrieve TIM instance and channel associated to pin
  // This is used to be compatible with all STM32 series automatically.
  TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(encoderPin, PinMap_PWM);
  channel = STM_PIN_CHANNEL(pinmap_function(encoderPin, PinMap_PWM));

  // Instantiate HardwareTimer object. Thanks to 'new' instantiation, HardwareTimer is not destructed when setup() function is finished.
  encoderTimer = new HardwareTimer(Instance);

  // Configure rising edge detection to measure frequency
  encoderTimer->setMode(channel, TIMER_INPUT_CAPTURE_RISING, encoderPin);

  // With a PrescalerFactor = 1, the minimum frequency value to measure is : TIM counter clock / CCR MAX
  //  = (SystemCoreClock) / 65535
  // Example on Nucleo_L476RG with systemClock at 80MHz, the minimum frequency is around 1,2 khz
  // To reduce minimum frequency, it is possible to increase prescaler. But this is at a cost of precision.
  // The maximum frequency depends on processing of the interruption and thus depend on board used
  // Example on Nucleo_L476RG with systemClock at 80MHz the interruption processing is around 4,5 microseconds and thus Max frequency is around 220kHz
  //uint32_t PrescalerFactor = 8;
  //encoderTimer->setPrescaleFactor(PrescalerFactor);
  //encoderTimer->setOverflow(0x10000); // Max Period value to have the largest possible time to detect rising edge and avoid timer rollover
  encoderTimer->setOverflow(2500000, MICROSEC_FORMAT);
  encoderTimer->attachInterrupt(channel, inputCapture_IT_callback);
  encoderTimer->attachInterrupt(rollover_IT_callback);
  encoderTimer->resume();

  channelOveflow = encoderTimer->getOverflow();

  // Compute this scale factor only once
  inputFreq = encoderTimer->getTimerClkFreq() / encoderTimer->getPrescaleFactor();
  inputFreq =  inputFreq / numberTicks; // Offset multiple ticks per rotation
}


