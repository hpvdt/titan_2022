void GPSCheck() {
  bool newData = false;

  while (GPSSERIAL.available()) {
    char c = GPSSERIAL.read();
    // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
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
  }
}
