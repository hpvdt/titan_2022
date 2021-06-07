#include <TinyGPS++.h>
//#include <SoftwareSerial.h>
/*
   This sample code demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = PA10, TXPin = PA9, gnd=PA8, vin=PB15;

//const int vin = 3;
//const int gnd = 4;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  pinMode(vin,OUTPUT);
  pinMode(gnd,OUTPUT);
  digitalWrite(vin,HIGH);
  digitalWrite(gnd,LOW);
  Serial.begin(9600);
  

}
void loop(){
  if (!Serial.available())
    Serial.println("No serial connection");
    
  while(Serial.available()){
  gps.encode(Serial.read());
  }
  Serial.print("Satellite Count:");
  Serial.println(gps.satellites.value());
  Serial.print("Speed: ");
  Serial.print(gps.speed.kmph()); 
  Serial.println("km/h");
  Serial.print("Altitude:");
  Serial.print(gps.altitude.meters());
  Serial.println("m");
  Serial.println(gps.location.lat(), 6);
  delay(1000);
}
