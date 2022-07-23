byte batteryLevel (char line) {
  // Factor by which to divide ADC reading (4095 at max, 3.3 ref)
  // Also need to voltage account for voltage division
  // Also need record each STM32's '3.3V' rail actual voltage
  const float readingToV = 1241.2121; // Given by 4096 over measured voltage
  const float fFactor = 4.3739; // Resistor voltage division factor
  const float rFactor = 4.3087;

  // Get the voltage on whichever line
  float reading = 0.0;
  switch (line) {
    case 'f':
      reading = float(analogRead(FBPin));
      reading /= readingToV;
      reading *= fFactor;
      break;
    case 'r':
      reading = float(analogRead(FBPin));
      reading /= readingToV;
      reading *= rFactor;
      break;
  }

  /* Convert voltage to % according to battery properties

    Uses linear interpolation to determine battery percentage.
    Starts from second highest set point (99%) and sees if the
    reading is higher than the set point. If it is, it'll
    interpolate using the set point and the next higher one.
    If the reading is less than the set point move to the next
    interval and repeat

    Voltages and percentages were sourced from:
    https://www.solacity.com/how-to-keep-lifepo4-lithium-ion-batteries-happy/
    And adjusted from 4 cells to 3 (by multiplying voltages by 0.75)
  */
  if (reading > 0) {
    const byte level[] = {100, 99, 90, 70, 40, 30, 20, 17, 14, 9, 0}; // Percentages linked to voltages
    const float voltage[] = {10.2, 10.05, 9.975, 9.9, 9.825, 9.75, 9.675, 9.6, 9.375, 9, 7.5}; // Voltages

    // Run though set point from
    for (byte i = 1; i <= 11; i++) {
      if (reading > voltage[i]) {
        // If the reading is in the region
        float temp = 0; // Used in calculations

        // Linear interpolation formula
        temp = (reading - voltage[i]) * float(level[i - 1] - level[i]);
        temp /= float(voltage[i - 1] - voltage[i]);
        temp += level[i];

        reading = temp; // Stores the result in the reading variable
        break;
      }
    }
  }
  reading = constrain(reading, 0, 100); // Constrain it to reasonable values
  
  return (int(reading));
}

void encoderDetect () {
  const unsigned long triggerTime = micros(); // Use micros instead of millis for higher precision

  if (triggerTime > (lastPass[previousTick] + debounce)) {
    // Checks if we have passed the debounce period from the last measurement

    speedKm = periodToKMH / (triggerTime - lastPass[currentTick]); // Gets speed from period
    
    lastPass[currentTick] = triggerTime; // Update trigger time for next run

    previousTick = currentTick; // Marks the current tick as the previous one for the next call
    // Using this variable reduces the calculations that occur when this routine is called

    // Updates tick indexes, cycling around when at the end
    if (currentTick == numberTicks - 1) {
      currentTick = 0;
      rotationCount++; // Increases rotation count with each full cycle
    }
    else currentTick++;
  }
  
}
