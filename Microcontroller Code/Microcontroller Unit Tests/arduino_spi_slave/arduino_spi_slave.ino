//SPI SLAVE (ARDUINO)
//SPI COMMUNICATION BETWEEN TWO ARDUINO
//CIRCUIT DIGEST
//Pramoth.T

#include<SPI.h>
volatile boolean received;
volatile byte Slavereceived, Slavesend;
int buttonvalue;
int x;
void setup() {
  pinMode(MISO, OUTPUT);            //Sets MISO as OUTPUT (Have to Send data to Master IN

  SPCR |= _BV(SPE);                 //Turn on SPI in Slave Mode
  received = false;

  SPI.attachInterrupt();            //Interrupt ON is set for SPI commnucation
}

ISR (SPI_STC_vect) {                //Interrupt routine function
  Slavereceived = SPDR;             // Value received from master if store in variable slavereceived
  received = true;                  //Sets received as True
  
  Slavereceived++;
  SPDR = 1;             //Sends the value to master via SPDR
}

void loop() {
  // If data was recieved
  if (received) {
    delay(1000);
  }
}
