#include "main.h"

const int numberFrames = 100; // Number of frames to render for testing
const float CIRCUMFERENCE = 2.104;

// Serial configuration
const bool useSerial = true; // Use serial or not
int serialLine = -1;

// ANT Configuration
const bool useANT = true; 	// Use ANT data from USB?

// Logging configuration
const bool enableLogging = true;

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
   
   fcntl(0, F_SETFL, O_NONBLOCK); // Set stdin (0) to be non-blocking
   
   // Start logging
   if (enableLogging == true) startLogging();
   
   for (int i = 0; i < numberFrames; i++) {    
      
      // ANT data
      // Checks for data, then if it is likely valid before reading
      if (useANT == true) {
         getANTData(ANTData, serialLine);
         
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
         
         //startTrial(); requestDataFloat(serialLine, 't', &temperature); endTrialIgnore("temperature", 40);
         //startTrial(); requestDataFloat(serialLine, 'h', &humidity); endTrialIgnore("humidity", 40);
         startTrial(); requestDataInt(serialLine, 'i', &battery); endTrialIgnore("battery", 40);
      
         startTrial();
         updateOverlay(speed, distance, power, cadence, heartRate, temperature, humidity, i);
         endTrialIgnore("overlay", 100);
         
         // Log data
         if (enableLogging == true) updateLog(speed, distance, ANTData[0], ANTData[5], 
                                             ANTData[1], ANTData[4], ANTData[0], ANTData[3], 
                                             temperature, humidity, battery, battery);
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
   printf("An average of %.3f realtime seconds for each frame (%.2f fps).\n", overallDelta / numberFrames, numberFrames / overallDelta);
   
   closeOverlay();
   
   return 0;
}