void GPSCheck() {
  bool newData = false;

  while (gpsSerial.available()) {
    char c = gpsSerial.read();
#ifdef ALLOW_DEBUG_SERIAL
  if (debugMode) {
    //DEBUGSERIAL.write(c); // See the GPS data flowing
  }
#endif
    if (gps.encode(c)) newData = true; // Did a new valid sentence come in?
  }
  if (newData) {
    unsigned long age;
    // Get coordinates
    gps.f_get_position(&latitude, &longitude, &age);
    latitude = (latitude == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : latitude);
    longitude = (longitude == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : longitude);
    
    altitudeGPS = gps.f_altitude();
    speedGPS = gps.f_speed_kmph();
    // Get the distance in KM from current to start point
    distanceGPS = (latitude == TinyGPS::GPS_INVALID_F_ANGLE ? 1000 : (unsigned long)TinyGPS::distance_between(latitude, longitude, startLat, startLong) / 1000);
    
#ifdef ALLOW_DEBUG_SERIAL
  if (debugMode) {
    DEBUGSERIAL.print("NEW GPS DATA: LAT: ");
    DEBUGSERIAL.print(latitude, 4);
    DEBUGSERIAL.print(" LONG: ");
    DEBUGSERIAL.print(longitude, 4);
    DEBUGSERIAL.print(" ALT: ");
    DEBUGSERIAL.print(altitudeGPS, 1);
    DEBUGSERIAL.print(" SPEED (KM/H): ");
    DEBUGSERIAL.print(speedGPS, 1);
    DEBUGSERIAL.print(" DIST TO START: ");
    DEBUGSERIAL.println(distanceGPS, 3);
  }
#endif
  }
}