#include "logging.h"

FILE * logFile;
char fileLocation[60];
struct timespec logStart, logCurrent;  // Realtime marks

void startLogging() {
	// Timers used to get start time
	time_t timer;
	struct tm* tm_info;
	timer = time(NULL);
	tm_info = localtime(&timer);
	
	
	char filename[40];
	strftime(filename, 40, "bikedata-%y%m%d-%H%M%S.csv", tm_info);
	
	printf("Log file name \'%s\' under ./bikelogs/ directory with vision code.\n", filename);
	sprintf(fileLocation, "./bikelogs/%s", filename);
	printf("If this seg faults soon, then check for a \"bikelogs\" directory and create it if missing.\n");
	
	logFile = fopen(fileLocation, "w+");
	fprintf(logFile, "Time (s),Speed (km/h),Distance (km),F. Power (w),R. Power (w),Total Power (w),F. Cadence,R. Cadence,F. Heart Rate,R. Heart Rate,Temperature (C),Humidity (%%),F. Battery Level (%%),R. Battery Level (%%)\n");
	fclose(logFile);
	
	clock_gettime(CLOCK_MONOTONIC, &logStart); // Mark start time
}

void updateLog(float spd, float dist, int fpwr, int rpwr, int fcad, int rcad, int fhr, int rhr, float temperature, float hum, float fbat, float rbat) {
	// Get elapsed time down to milliseconds 
	clock_gettime(CLOCK_MONOTONIC, &logCurrent);
	u_int64_t timeElapsedMS = ((logCurrent.tv_sec - logStart.tv_sec) * 1000) + ((logCurrent.tv_nsec - logStart.tv_nsec) / 1000000);
	float timeElapsed = timeElapsedMS / 1000.0; // Convert to seconds
	
	// Open and append data
	logFile = fopen(fileLocation, "a"); // Append
	fprintf(logFile, "%.3f,%.3f,%.3f,%d,%d,%d,%d,%d,%d,%d,%.1f,%.1f,%.f,%.f\n",
				timeElapsed, spd, dist, fpwr, rpwr, fpwr + rpwr, fcad, 
				rcad, fhr, rhr, temperature, hum, fbat, rbat);
	fclose(logFile);
}