#ifndef TITAN_GPS_HEADER
#define TITAN_GPS_HEADER

#include <Arduino.h>
#include <TinyGPS.h>
#include "communication.h"

// Variables
extern float latitude;         // Latitude (degrees)
extern float longitude;        // Longitude (degrees)
extern float altitudeGPS;      // Altitude (m)
extern float speedGPS;         // GPS speed in km/h
extern float distanceGPS;      // GPS distance in km

// Coordinates that mark the start point (used for getting distance by GPS) 
extern float startLong;
extern float startLat;

void GPSCheck();

#endif
