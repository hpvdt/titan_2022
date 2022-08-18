#include "logging.h"

//#define USE_TIME_FOR_FILE_NAME // Use only if connected to internet or NTC installed

FILE * logFile;
char fileLocation[70];

struct timespec logStart, logCurrent;  // Realtime marks for keeping track of ride time

void startLogging() {
	
	char filename[40]; // Used for making file NAME, not recording LOCATION!

#ifdef USE_TIME_FOR_FILE_NAME
	// Base the file name on the present time
	// Timers used to get start time
	time_t timer;
	timer = time(NULL);
	struct tm* tm_info;
	
	tm_info = localtime(&timer);
	
	strftime(filename, 40, "bikedata-%y%m%d-%H%M%S.csv", tm_info);
	
#else
	// Without accurate time, just index a number on the file
	// Based on the number of files (logs) present in the log folder

	// File counting code from:
	// https://stackoverflow.com/questions/1121383/counting-the-number-of-files-in-a-directory-using-c
	int fileCount = 0;
	DIR * dirp;
	struct dirent * entry;

	dirp = opendir("./bikelogs/");
	while ((entry = readdir(dirp)) != NULL) {
		  if (entry->d_type == DT_REG) { // If the entry is a regular file
		       fileCount++;
		  }
	}
	closedir(dirp);
	
	sprintf(filename, "bikedata-%03d.csv", fileCount);
#endif

	printf("Log file name \'%s\' under ./bikelogs/ directory with vision code.\n", filename);
	sprintf(fileLocation, "./bikelogs/%s", filename);
	printf("If this seg faults soon, then check for a \"bikelogs\" directory and create it if missing.\n");
	
	logFile = fopen(fileLocation, "w+");
	fprintf(logFile, "Time (s),Speed (km/h),Distance (km),F. Power (w),R. Power (w),Total Power (w),F. Cadence,R. Cadence,F. Heart Rate,R. Heart Rate,Temperature (C),Humidity (%%),F. Battery Level (%%),R. Battery Level (%%),F. Brake Temp (C),R. Brake Temp (C),CO2 (ppm), Performance Factor (%%)\n");
	fclose(logFile);
	
	clock_gettime(CLOCK_MONOTONIC, &logStart); // Mark start time
}

void updateLog(float spd, float dist, int fpwr, int rpwr, int fcad, int rcad, int fhr, int rhr, float temperature, float hum, float fbat, float rbat, float fbrk, float rbrk, int CO2, float perf) {
	// Get elapsed time down to milliseconds 
	clock_gettime(CLOCK_MONOTONIC, &logCurrent);
	u_int64_t timeElapsedMS = ((logCurrent.tv_sec - logStart.tv_sec) * 1000) + ((logCurrent.tv_nsec - logStart.tv_nsec) / 1000000);
	float timeElapsed = timeElapsedMS / 1000.0; // Convert to seconds
	
	// Open and append data
	logFile = fopen(fileLocation, "a"); // Append
	fprintf(logFile, "%.3f,%.3f,%.3f,%d,%d,%d,%d,%d,%d,%d,%.1f,%.1f,%.f,%.f,%.f,%.f,%d,%.1f\n",
				timeElapsed, spd, dist, fpwr, rpwr, fpwr + rpwr, fcad, 
				rcad, fhr, rhr, temperature, hum, fbat, rbat, fbrk, 
				rbrk, CO2, perf);
	fclose(logFile);
}