#include "main.h"

const int numberFrames = 100; // Number of frames to render for testing
const float CIRCUMFERENCE = 2.104;

// Serial variables
const bool useSerial = true; // Use serial or not
int serialLine = -1;

// ANT Configuration
const bool useANT = true; 	// Use ANT data from USB?

int main() {
   // Local variables
   int cadence = 0; 
   int power = 0;
   int heartRate = 0;
   int battery = 0;
   int rotations = 0;
   float speed = 0.0;
   float distance = 0.0;
   float temperature = 0.0;
   float humidity = 0.0;
   int ANTData[] = {0,0,0,0,0,0};

   startOverlay(false);  // Camera off
   
   time_t t,t2; // Realtime marks
   time(&t); // Get start time
   clock_t c = clock(); // Get start processor time
   
   // Open serial line
   if (useSerial == true) {
      serialLine = openLine("/dev/serial0");

      if (serialLine != -1) printf("SUCCESSFULLY OPENED SERIAL LIN\n");
   }
   else printf("NOT USING SERIAL DATA. JUST A TEST\n");

   // Inform if ANT data is expected or not
   if (useANT == true) printf("EXPECTING ANT DATA PIPED IN\n");
   else printf("NOT USING ANT DATA. JUST MAKING STUFF UP.\n");

   
   for (int i = 0; i < numberFrames; i++) {    
      
      // ANT data
      if (useANT == true) {
         getANTData(ANTData, serialLine, useSerial);
         
         // Use data for front
         heartRate = ANTData[0];
         cadence = ANTData[1];
         power = ANTData[2];
      }
      
      
      // Serial Data
      if (useSerial == true) {
         // Request data
         // Actual bike running
         if (useANT == false) {
            // Check with microcontroller (in case it is being fed test data)
            requestDataInt(serialLine, 'a', &heartRate); 
            requestDataInt(serialLine, 'c', &cadence);
            requestDataInt(serialLine, 'e', &power);
         }
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
         updateOverlayTest(i, power, cadence, heartRate);
      }
   }
   c = clock() - c; // Get run time
   time(&t2); // Get end time
   
   closeOverlay();
   
   float secondsElapsed = ((float) c)/CLOCKS_PER_SEC;
   
   printf("\nIt took %f processor seconds, %.f realtime seconds for %d frames.\n", secondsElapsed, difftime(t2,t), numberFrames);
   return 0;
}

