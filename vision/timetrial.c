#include "timetrial.h"

// Variables used for time trials
struct timespec tStart,tEnd;  // Realtime marks
clock_t processClock;         // Process clock

void startTrial() {
   clock_gettime(CLOCK_MONOTONIC, &tStart); // Get start time
   processClock = clock(); // Get start processor time
}

u_int64_t endTrial(char * nameOfTrial) {
   processClock = clock() - processClock; // Get run time
   clock_gettime(CLOCK_MONOTONIC, &tEnd); // Get end time
   float secondsElapsed2 = ((float) 1000.0 * processClock)/CLOCKS_PER_SEC;
   
   u_int64_t delta = ((tEnd.tv_sec - tStart.tv_sec) * 1000) + ((tEnd.tv_nsec - tStart.tv_nsec) / 1000000);
   
   printf("It took %4.3f processor milliseconds, %lld realtime milliseconds for that \'%s\' trial.\n", secondsElapsed2, delta, nameOfTrial);
   return delta;
}

u_int64_t endTrialIgnore(char * nameOfTrial, u_int64_t ignoreLimit) {
   processClock = clock() - processClock; // Get run time
   clock_gettime(CLOCK_MONOTONIC, &tEnd); // Get end time
   float secondsElapsed2 = ((float) 1000.0 * processClock)/CLOCKS_PER_SEC;
   
   u_int64_t delta = ((tEnd.tv_sec - tStart.tv_sec) * 1000) + ((tEnd.tv_nsec - tStart.tv_nsec) / 1000000);
   
   if (delta > ignoreLimit) {
		printf("It took %4.3f processor milliseconds, %lld realtime milliseconds for that \'%s\' trial.\n", secondsElapsed2, delta, nameOfTrial);
   }
   return delta;
}