#include "main.h"

const int numberFrames = 100; // Number of frames to render for testing
const float CIRCUMFERENCE = 2.104;

// Serial configuration
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
   
   // Timing the entire process
   struct timespec tSystemStart,tSystemEnd;
   clock_gettime(CLOCK_MONOTONIC, &tSystemStart); // Get start time
   clock_t titanProcessClock = clock(); // Get start processor time
   
   // Open serial line
   if (useSerial == true) {
      serialLine = openLine("/dev/serial0");
      
      if (serialLine != -1) printf("SUCCESSFULLY OPENED SERIAL LINE\n");
   }
   else printf("NOT USING SERIAL DATA. JUST A TEST..\n");
   
   // Inform if ANT data is expected or not
   if (useANT == true) printf("EXPECTING ANT DATA PIPED IN\n");
   else printf("NOT USING ANT DATA. JUST MAKING STUFF UP.\n");
   
   for (int i = 0; i < numberFrames; i++) {    
      
      // ANT data
      // Checks for data, then if it is likely valid before reading
      if (useANT == true) {
         
         startTrial();
         getANTData(ANTData, serialLine);
         endTrial("ANT");
         
         
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
         
         startTrial(); requestDataFloat(serialLine, 's', &speed);  endTrialIgnore("speed", 40);
         startTrial(); requestDataInt(serialLine, 'q', &rotations); endTrialIgnore("rotations", 40);
         
         distance = rotations * CIRCUMFERENCE;
         
         temperature = 25.5;//requestDataFloat(serialLine, 't', &temperature); 
         humidity = 60.0;//requestDataFloat(serialLine, 'h', &humidity); 
         startTrial();
         requestDataInt(serialLine, 'i', &battery); 
         endTrialIgnore("battery", 40);
      
         startTrial();
         updateOverlay(speed, distance, power, cadence, heartRate, temperature, humidity, i);
         endTrialIgnore("overlay", 50);
      }
      else {
         // Just test the overlay
         updateOverlayTest(i, power, cadence, heartRate);
      }
   }
   
   // Overal time
   titanProcessClock = clock() - titanProcessClock; // Get run time
   clock_gettime(CLOCK_MONOTONIC, &tSystemEnd); // Get end time
   float secondsElapsed = ((float) titanProcessClock)/CLOCKS_PER_SEC;
   
   float overallDelta = (tSystemEnd.tv_sec - tSystemStart.tv_sec) + ((tSystemEnd.tv_nsec - tSystemStart.tv_nsec) / 1000000000.0);
   
   printf("\nIt took %2.3f processor seconds, %.3f realtime seconds for %d frames.\n", secondsElapsed, overallDelta, numberFrames);
   
   closeOverlay();
   
   
   return 0;
}