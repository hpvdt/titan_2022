#include "main.h"

int main() {
   startOverlay(); 
   
   time_t t,t2; // Realtime marks
   time(&t); // Get start time
   clock_t c = clock(); // Get start processor time
   
   // Open serial line
   if (useSerial == true) {
      serialLine = openLine("/dev/serial0");
   }
   
   for (int i = 0; i < numberFrames; i++) {    
      
      if (useSerial == true) {
         // Request data
         // Actual bike running
         requestDataInt(serialLine, 'a', &heartRate); 
         requestDataInt(serialLine, 'c', &cadence); 
         requestDataInt(serialLine, 'e', &power); 
         requestDataFloat(serialLine, 's', &speed); 
         requestDataInt(serialLine, 'q', &rotations);
         distance = rotations * CIRCUMFERENCE;
         
         requestDataFloat(serialLine, 't', &temperature); 
         requestDataFloat(serialLine, 'h', &humidity); 
         requestDataInt(serialLine, 'i', &battery); 
         
         updateOverlay(speed, distance, i, cadence, heartRate, temperature, humidity, battery);
      }
      else {
         // Just test the overlay
         updateOverlayTest(i);
      }
   }
   c = clock() - c; // Get run time
   time(&t2); // Get end time
   
   closeOverlay();
   
   float secondsElapsed = ((float) c)/CLOCKS_PER_SEC;
   
   printf("\nIt took %f processor seconds, %.f realtime seconds for %d frames.\n", secondsElapsed, difftime(t2,t), numberFrames);
   return 0;
}

