#include "antInterface.h"

//#define ANT_DEBUG_MESSAGES // Define for debug messages

const int dataFieldWidth = 4; // Include delimiter!
const int numANTDataFields = 6;
#define antBufferLength 60

char ANTBuffer[antBufferLength]; 

bool getANTDataPipedIn(int *dataOut, int serialLn) {
	// Check if there is data available or stdin isn't broken
	if (fgets(ANTBuffer, antBufferLength, stdin) != NULL) {
		
		// Check if it is text and skip
		// Numeric data should start with a number
		if ((ANTBuffer[0] < '0') || (ANTBuffer[0] > '9')) {
#ifdef ANT_DEBUG_MESSAGES
			printf("ANT data didn't begin with a number. Contents until a number:\n");
#endif
			
			do {
#ifdef ANT_DEBUG_MESSAGES
				printf("\t%s", ANTBuffer);
#endif
				if (fgets(ANTBuffer, antBufferLength, stdin) == NULL) return(-1);
			} while ((ANTBuffer[0] < '0') || (ANTBuffer[0] > '9'));
		}

		// Data is always set in a set length
		for (int i = 0; i < numANTDataFields; i++) {

			char tempBuffer[dataFieldWidth];
			tempBuffer[0] = ANTBuffer[(i * dataFieldWidth)];
			tempBuffer[1] = ANTBuffer[(i * dataFieldWidth) + 1];
			tempBuffer[2] = ANTBuffer[(i * dataFieldWidth) + 2];
			tempBuffer[dataFieldWidth-1] = '\0';
			
			dataOut[i] = atoi(tempBuffer);
		}
		
		if (serialLn != -1) { 
			// Send everthing at once
			sprintf(ANTBuffer, "%03d,%03d,%03d,%03d,%03d,%03d", dataOut[0], dataOut[3], dataOut[1], dataOut[4], dataOut[2], dataOut[5]);
			sendData(serialLn, 'G', ANTBuffer);
		}
		
#ifdef ANT_DEBUG_MESSAGES
		printf("ANT DATA IN: %03d,%03d,%03d|%03d,%03d,%03d\n", dataOut[0], dataOut[1], dataOut[2], dataOut[3], dataOut[4], dataOut[5]);
#endif
		return(0);
	}
	else {
#ifdef ANT_DEBUG_MESSAGES
		printf("No data on stdin for ANT or it is broken.\n");
#endif
		return(-1);
	}
}

bool getANTDataSerial(int *dataOut, int serialLn){
	
	// Check for a valid serial line before requesting
	if (serialLn == -1) {
#ifdef ANT_DEBUG_MESSAGES
		printf("No serial line to request ANT data from.\n");
#endif
		return false; 
	}
	
	char ANTBuffer[50];
	requestData(serialLn, 'g', ANTBuffer);
	
	
#ifdef ANT_DEBUG_MESSAGES
	printf("Recieved ANT data: '%s'\n", ANTBuffer);
#endif
	
	// Go through recieved string to split it at commas (message should never exceed 24)
	char tempBuffer[6][4];	// Used to store each ANT field temporarily as strings
	
	sscanf(ANTBuffer, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", tempBuffer[0], tempBuffer[1], tempBuffer[2], tempBuffer[3], tempBuffer[4], tempBuffer[5]);
	// '%[^,]' acts like '%s' but it reads until a comma is detected ("read anything excluding ',')
	// This is why I still have commas in the search string, so they are skipped over before the next search
	
	// Convert strings to integers
	for (int i = 0; i < 6; i++) dataOut[i] = atoi(tempBuffer[i]);
	
#ifdef ANT_DEBUG_MESSAGES
	printf("Parsed ANT data:\n");
	for (int i = 0; i < 6; i++) printf("\t%03d\n", dataOut[i]);
#endif
	
	return true;
}