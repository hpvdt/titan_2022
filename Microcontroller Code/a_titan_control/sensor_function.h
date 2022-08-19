#include <Adafruit_MLX90614.h>
#include "DHT.h" // Used for temperature and humidity (Adafruit DHT library)
#include <TinyGPS.h>

const byte frontBrakeAdd = 0x01;
const byte rearBrakeAdd = 0x02;

const float frontEmissivity = 1.0;
const float rearEmissivity = 1.0;

// Brake temperatures
float frontBrakeTemp = 0;
float rearBrakeTemp = 0;

unsigned long brakeTime = 0;
unsigned long brakePeriod = 1000;

Adafruit_MLX90614 frontBrake = Adafruit_MLX90614();
Adafruit_MLX90614 rearBrake = Adafruit_MLX90614();