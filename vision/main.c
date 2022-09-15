#include "main.h"
const int powerAvgFrames = 50;

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
   printf(ANSI_COLOUR_BLUE "\n==================\n! TITAN 2022 HUD !\n==================" ANSI_COLOUR_RESET);
   
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
   printf("\nParsed configuration:\n" ANSI_COLOUR_YELLOW);
   if (isFront == true) printf("\tRole: FRONT\n");
   else printf("\tRole: REAR\n");
   printf("\tCamera running: %d\n", enableCamera);
   printf("\tCollecting ANT data: %d\n", collectANT);
   printf("\tCommunicate over serial: %d\n", useSerial);
   printf("\tLogging: %d\n", enableLogging);
   if (numberFrames > 0) printf("\tNumber of frames: %d\n\n" ANSI_COLOUR_RESET, numberFrames);
   else printf("\tNo limit to number of frames.\n\n" ANSI_COLOUR_RESET);
   
   // Local variables
   int frontCadence = 0, rearCadence = 0; 
   int frontPower = 0, rearPower = 0;
   int frontHeartRate = 0, rearHeartRate = 0;
   int ANTData[] = {0,0,0,0,0,0};
   int rearBattery = 0, frontBattery = 0;
   int rotations = 0;
   float speedWheel = 0.0;
   float distanceWheel = 0.0, distanceGPS = 0.0;
   float temperature = 0.0;
   float humidity = 0.0;
   float performanceFactor = 0.0;
   float frontBrakeTemp = 200.0, rearBrakeTemp = 200.0;
   int ppmCO2 = 0;
   float speedGPS = 0.0;
   
   // Open serial line
   if (useSerial == true) {
      serialLine = openLine("/dev/serial0");
      
      if (serialLine != -1) printf(ANSI_COLOUR_GREEN "Successfully opened serial line!\n" ANSI_COLOUR_RESET);
      else {
         printf(ANSI_COLOUR_RED "FAILED TO OPEN SERIAL LINE!\nReverting to non-serial behavior.\n" ANSI_COLOUR_RESET);
         useSerial = false;
      }
   }
   else printf(ANSI_COLOUR_RED "NOT USING SERIAL DATA.\n" ANSI_COLOUR_RESET);
   
   // Inform if ANT data is expected, and set up stdin accordingly
   if (collectANT == true) {
      printf("Expecting ANT data to be piped in.\n");
      fcntl(0, F_SETFL, O_NONBLOCK); // Set stdin (0) to be non-blocking
   }
   else if (useSerial == true) printf(ANSI_COLOUR_GREEN "Not collecting ANT data locally, requesting ANT data over serial.\n" ANSI_COLOUR_RESET);
   else printf(ANSI_COLOUR_RED "NOT USING ANY ANT DATA. DISPLAYING RANDOM ANT DATA!\n" ANSI_COLOUR_RESET);
   
   // Start RaceSim
   setupRaceSim();
   
   // Start logging
   if (enableLogging == true) startLogging();
   else printf(ANSI_COLOUR_YELLOW "NOT LOGGING DATA!\n" ANSI_COLOUR_RESET);
   
   // Print camera status
   if (enableCamera == true) printf(ANSI_COLOUR_RESET "Camera will be launched shortly!\n" ANSI_COLOUR_RESET);
   else printf(ANSI_COLOUR_RED "Camera not configured to start.\n" ANSI_COLOUR_RESET);
      
   sleep(3); // Pause to show config before potentially running camera
   
   printf("\n=========================================================\n");
   printf("RUNNING MAIN LOOP");
   printf("\n=========================================================\n\n");
   
   
   // Timing the entire process
   struct timespec tSystemStart,tSystemEnd;
   clock_gettime(CLOCK_MONOTONIC, &tSystemStart); // Get start time
   clock_t titanProcessClock = clock(); // Get start processor time
   
   startOverlay(enableCamera);
   
   // Main HUD loop
   int framesRemaining = numberFrames;
   int powerFrameCounter = 0; 
   int frontPowerSum = 0;
   int rearPowerSum = 0;  
   int prevFrontPower = 0;
   int prevRearPower = 0; 
   do {
      
      // ANT data
      printf("Grabbing ANT data\n");
      if (collectANT == true) getANTDataPipedIn(ANTData, serialLine);      // It is meant to collect data locally
      else if (useSerial == false) { 
         // Cant' collect nor request ANT data, use random numbers
         ANTData[0] = 60;
         ANTData[1] = 60;
         ANTData[2] = 120;
         ANTData[3] = 60;
         ANTData[4] = 60;
         ANTData[5] = 120;
      }
      
      // Copy to nicer named variables
      frontHeartRate = ANTData[0];
      frontCadence = ANTData[1];
      frontPower = ANTData[2];
      rearHeartRate = ANTData[3];
      rearCadence = ANTData[4];
      rearPower = ANTData[5];

      
      // Get bike data
      if (useSerial == true) { // Collect bike data from STM32 over serial      
         printf("Grabbing serial data\n");
         
         char bulkBuffer[32];
         startTrial(); requestData(serialLine, '{', &bulkBuffer[1]); 
         
         struct bulkDataStruct {
            char messageType;
            char messageLength;
            uint16_t distGPS;
            uint32_t speedEncoder;
            uint32_t speedGPS;
            uint16_t rotations;
            uint16_t frontBrakeT;
            uint16_t rearBrakeT;
            uint8_t fBatt;
            uint8_t rBatt;
            uint8_t humid;
            uint8_t temp;
            uint16_t CO2;
            uint8_t fhr;
            uint8_t rhr;
            uint8_t fcad;
            uint8_t rcad;
            uint16_t fpwr;
            uint16_t rpwr;
         } dataLoad;
         
         memcpy(&dataLoad, bulkBuffer, sizeof(dataLoad));
         dataLoad.messageType = '[';
         
         distanceGPS = dataLoad.distGPS / 1000.0;
         speedWheel = dataLoad.speedEncoder / 1000.0;
         speedGPS = dataLoad.speedGPS / 1000.0;
         rotations = dataLoad.rotations;
         frontBrakeTemp = dataLoad.frontBrakeT / 100.0;
         rearBrakeTemp =dataLoad.rearBrakeT / 100.0;
         frontBattery = dataLoad.fBatt;
         rearBattery = dataLoad.rBatt;
         humidity = dataLoad.humid / 2.0;
         temperature = (dataLoad.temp - 50.0) / 2.0;
         ppmCO2 = dataLoad.CO2;
         frontHeartRate = dataLoad.fhr;
         rearHeartRate = dataLoad.rhr;
         frontCadence = dataLoad.fcad;
         rearCadence = dataLoad.rcad;
         frontPower = dataLoad.fpwr;
         rearPower = dataLoad.rpwr;

         endTrialIgnore("Bulk transfer", 30);

         distanceWheel = rotations * CIRCUMFERENCE / 1000.0;
         
         /*
         printf("Message type: %c, length char %c\n", dataLoad.messageType, dataLoad.messageLength);
         printf("\tSpeeds: %.3f / %.3f\n", speedWheel, speedGPS);
         printf("\tDist: %.3f / %.3f\n", distanceWheel, distanceGPS);
         printf("\tBrake Temps: %.2f / %.2f\n", frontBrakeTemp, rearBrakeTemp);
         printf("\tBatteries: %d / %d\n", frontBattery, rearBattery);
         printf("\tAtmosphere: %.1f degC, %.1f %%RH, %d ppmCO2\n", temperature, humidity, ppmCO2);
         printf("\tANT front: HR %d | CAD %d | PWR %d\n", frontHeartRate, frontCadence, frontPower);
         printf("\tANT rear: HR %d | CAD %d | PWR %d\n", rearHeartRate, rearCadence, rearPower);
         */
         
      }
      else {
         // Placeholder data for when not collecting anything over serial
         rearBattery = 25;
         frontBattery = 25;
         speedWheel = 120.6;
         distanceWheel = 7;
         temperature = 0.0;
         humidity = 0.0;
         performanceFactor = 101.2;
         frontBrakeTemp = 200.0;
         rearBrakeTemp = 200.0;
         ppmCO2 = 1550;
         speedGPS = 120.6;
         distanceGPS = 6.92;
      }
      
      printf("Averaging power\n");
      // Get average power over powerAvgFrames frames 
      // 10 frames/second, power sensor polls 1 times/second 
      if (powerFrameCounter < powerAvgFrames){
         frontPowerSum += frontPower;
         rearPowerSum += rearPower;  
         frontPower = prevFrontPower;
         rearPower = prevRearPower;
         powerFrameCounter ++; 
      }
      else if (powerFrameCounter == powerAvgFrames){
         powerFrameCounter = 0;
         frontPower = frontPowerSum / powerAvgFrames;
         frontPowerSum = 0;
         rearPower = rearPowerSum / powerAvgFrames;
         rearPowerSum = 0;
         prevFrontPower = frontPower;
         prevRearPower = rearPower;
      }
      else {
         printf("you done screw up on the power loop.");
      }
      
      
      printf("Performance factor\n");
      // Performance factor
      performanceFactor = compareToSimulation(speedWheel, distanceWheel, (frontPower + rearPower));

      
      // Overlays
      printf("Making overlay\n");
      if (isFront == true) { // Front overlay
         startTrial();
         updateOverlayFront(speedWheel, distanceWheel, frontPower, frontCadence, frontHeartRate, performanceFactor, frontBrakeTemp, frontBattery, speedGPS);
         endTrialIgnore("front overlay", 100);
      }
      else { // Rear overlay
         startTrial();
         updateOverlayRear(speedWheel, distanceWheel, rearPower, frontPower, rearCadence, rearHeartRate, frontBrakeTemp, rearBrakeTemp, rearBattery, performanceFactor, ppmCO2, speedGPS);
         endTrialIgnore("rear overlay", 100);
      }
      
      
      // Logging
      if (enableLogging == true) {
         printf("Logging\n");
         updateLog(speedWheel, distanceWheel, frontPower, rearPower, 
                  frontCadence, rearCadence, frontHeartRate, rearHeartRate, 
                  temperature, humidity, frontBattery, rearBattery,
                  frontBrakeTemp, rearBrakeTemp, ppmCO2, performanceFactor,
                  speedGPS, distanceGPS);
      }
      
      // Count down number of frames if there was a limit stated
      if (numberFrames > 0) framesRemaining--;
   } while ((framesRemaining != 0) || (numberFrames == -1));
   
   // Overal time
   titanProcessClock = clock() - titanProcessClock; // Get run time
   clock_gettime(CLOCK_MONOTONIC, &tSystemEnd); // Get end time
   float secondsElapsed = ((float) titanProcessClock)/CLOCKS_PER_SEC;
   
   float overallDelta = (tSystemEnd.tv_sec - tSystemStart.tv_sec) + ((tSystemEnd.tv_nsec - tSystemStart.tv_nsec) / 1000000000.0);
   
   printf(ANSI_COLOUR_MAGENTA "\nIt took %2.3f processor seconds, %.3f realtime seconds for %d frames.\n" ANSI_COLOUR_RESET, secondsElapsed, overallDelta, numberFrames);
   printf(ANSI_COLOUR_MAGENTA "An average of %.3f realtime seconds for each frame (%.2f fps).\n" ANSI_COLOUR_RESET, overallDelta / numberFrames, numberFrames / overallDelta);
   
   closeOverlay();
   
   return 0;
}