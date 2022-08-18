#include "main.h"

int numberFrames = -1; // Number of frames to render for testing (-1 for infinite)
const float CIRCUMFERENCE = 2.104;

// Serial configuration
bool useSerial = false; // Use serial or not
int serialLine = -1;

bool collectANT = false; 	   // Use ANT data from USB?
bool enableLogging = false;   // Logging configuration
bool enableCamera = false;    // Camera
bool isFront = true;          // Is front or not

int main(int argc, char *argv[]) {
   printf("\n\n! TITAN 2022 HUD !\n");
   
   // Start by reading in arguements
   printf("\n\nExpected arguements when called in order: 'setup string' 'number of frames'\n");
   printf("\tSpecify role with 'f' or 'r' (assumed to be front if not specified)\
         \n\tEnable camera with 'c'\n\tEnable logging with 'l'\
         \n\tEnable ANT collection with 'a'\n\tEnable serial coms with 's'\
         \n\n\tFrames as -1 for infinty, otherwise the number wanted.\
         \n\tE.g. 'bike.bin fc 10' sets system for front operation with camera for 10 frames\n");
   
   // Read in configuration using a run through switch based on arguement count
   // Note: arguement 0 is the call to run the program itself e.g. "./bike.bin"
   int characterToRead = 0;
   switch (argc) {
      case 3:
         // Get frame count
         numberFrames = atoi(argv[2]);
      case 2:
         // Go through setup string
         do {
            switch (argv[1][characterToRead]) {
               case 'l':
                  enableLogging = true;
                  break;
               case 's':
                  useSerial = true;
                  break;
               case 'a':
                  collectANT = true;
                  break;
               case 'c':
                  enableCamera = true;
                  break;
               case 'r':
                  isFront = false;
                  break;
               case 'f':
                  isFront = true;
                  break;
            }
         } while (argv[1][characterToRead++] != '\0'); 
         break;
   }
   

   
   // Reguritate configuration
   printf("Parsed configuration:\n");
   if (isFront == true) printf("Role: FRONT\n");
   else printf("Role: REAR\n");
   printf("Camera running: %d\n", enableCamera);
   printf("Collecting ANT data: %d\n", collectANT);
   printf("Communicate over serial: %d\n", useSerial);
   printf("Logging: %d\n", enableLogging);
   printf("Number of frames: %d\n\n", numberFrames);
   
   // Local variables
   int cadence = 0; 
   int power = 0;
   int heartRate = 0;
   int rearBattery = 0, frontBattery = 0;
   int rotations = 0;
   float speed = 0.0;
   float distance = 0.0;
   float temperature = 0.0;
   float humidity = 0.0;
   int ANTData[] = {0,0,0,0,0,0};
   float performanceFactor = 0.0;
   float frontBrakeTemp = 200.0, rearBrakeTemp = 200.0;
   int ppmCO2 = 0;
   
   // Open serial line
   if (useSerial == true) {
      serialLine = openLine("/dev/serial0");
      
      if (serialLine != -1) printf("Successfully opened serial line!\n\n");
      else {
         printf("FAILED TO OPEN SERIAL LINE!\nReverting to non-serial behavior.\n\n");
         useSerial = false;
      }
   }
   else printf("NOT USING SERIAL DATA.\n\n");
   
   // Inform if ANT data is expected, and set up stdin accordingly
   if (collectANT == true) {
      printf("Expecting ANT data to be piped in.\n\n");
      fcntl(0, F_SETFL, O_NONBLOCK); // Set stdin (0) to be non-blocking
   }
   else printf("NOT USING ANT DATA. DISPLAYING RANDOM ANT DATA!\n\n");
   
   // Start logging
   if (enableLogging == true) startLogging();
   else printf("NOT LOGGING DATA!\n\n");
   
   
   
   sleep(5); // Pause to show config before potentially running camera
   
   // Timing the entire process
   struct timespec tSystemStart,tSystemEnd;
   clock_gettime(CLOCK_MONOTONIC, &tSystemStart); // Get start time
   clock_t titanProcessClock = clock(); // Get start processor time
   
   startOverlay(enableCamera);
   
   // Main HUD loop
   int framesRemaining = numberFrames;
   do {
      
      // ANT data
      if (collectANT == true) { // Is meant to collect data locally
         getANTData(ANTData, serialLine);
         
         // Record relevant local data
         if (isFront == true) {
            heartRate = ANTData[0];
            cadence = ANTData[1];
            power = ANTData[2];
         }
         else {
            heartRate = ANTData[3];
            cadence = ANTData[4];
            power = ANTData[5];
         }
         
      }
      else if (useSerial == true) { // Not collecting ANT data locally, but can use serial to request it
         
         // Record all to ANT data for logging on either station
         requestDataInt(serialLine, 'a', &ANTData[0]); 
         requestDataInt(serialLine, 'c', &ANTData[1]);
         requestDataInt(serialLine, 'e', &ANTData[2]);
         requestDataInt(serialLine, 'b', &ANTData[3]); 
         requestDataInt(serialLine, 'd', &ANTData[4]);
         requestDataInt(serialLine, 'f', &ANTData[5]);
         // TODO: Change this to use the call for all ANT data 'g'
         
         // Record relevant local data
         if (isFront == true) {
            heartRate = ANTData[0];
            cadence = ANTData[1];
            power = ANTData[2];
         }
         else {
            heartRate = ANTData[3];
            cadence = ANTData[4];
            power = ANTData[5];
         }
      }   
      else { // Cant' collect nor request ANT data, use random numbers
         heartRate = 60;
         cadence = 60;
         power = 120;
      }
      
      
      // Get bike data
      if (useSerial == true) { // Collect bike data from STM32 over serial      
         startTrial(); requestDataFloat(serialLine, 's', &speed);  endTrialIgnore("speed", 40);
         startTrial(); requestDataInt(serialLine, 'q', &rotations); endTrialIgnore("rotations", 40);
         
         distance = rotations * CIRCUMFERENCE;
         
         startTrial(); requestDataFloat(serialLine, 't', &temperature); endTrialIgnore("temperature", 40);
         startTrial(); requestDataFloat(serialLine, 'h', &humidity); endTrialIgnore("humidity", 40);
         startTrial(); requestDataInt(serialLine, 'i', &frontBattery); endTrialIgnore("front battery", 40);
         startTrial(); requestDataInt(serialLine, 'j', &rearBattery); endTrialIgnore("rear battery", 40);
         
         startTrial(); requestDataFloat(serialLine, 'w', &frontBrakeTemp); endTrialIgnore("front brake temperature", 40);
         startTrial(); requestDataFloat(serialLine, 'x', &rearBrakeTemp); endTrialIgnore("rear brake temperature", 40);
         startTrial(); requestDataInt(serialLine, 'k', &ppmCO2); endTrialIgnore("CO2", 40);
      }

      
      // Overlays
      if (isFront == true) { // Front overlay
         startTrial();
         updateOverlayFront(speed, distance, power, cadence, heartRate, performanceFactor, frontBrakeTemp, frontBattery);
         endTrialIgnore("front overlay", 100);
      }
      else { // Rear overlay
         startTrial();
         updateOverlayRear(speed, distance, power, ANTData[2], cadence, heartRate, frontBrakeTemp, rearBrakeTemp, rearBattery, performanceFactor, ppmCO2);
         endTrialIgnore("rear overlay", 100);
      }
      
      
      // Logging
      if (enableLogging == true) updateLog(speed, distance, ANTData[0], ANTData[5], 
                                             ANTData[1], ANTData[4], ANTData[0], ANTData[3], 
                                             temperature, humidity, frontBattery, rearBattery,
                                             frontBrakeTemp, rearBrakeTemp, ppmCO2, performanceFactor);
      
      // Count down number of frames if there was a limit stated
      if (numberFrames > 0) framesRemaining--;
   } while (framesRemaining > 0);
   
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