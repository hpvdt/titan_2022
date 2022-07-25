#include "antInterface.h"

//#define ANT_DEBUG_MESSAGES // Define for debug messages

const int dataFieldWidth = 4; // Include delimiter!
const int numANTDataFields = 6;
#define antBufferLength 60

char ANTBuffer[antBufferLength];

bool getANTData(int *dataOut, int serialLn) {
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

			// TODO: Make a loop based on dataFieldWidth?
			char tempBuffer[dataFieldWidth];
			tempBuffer[0] = ANTBuffer[(i * dataFieldWidth)];
			tempBuffer[1] = ANTBuffer[(i * dataFieldWidth) + 1];
			tempBuffer[2] = ANTBuffer[(i * dataFieldWidth) + 2];
			tempBuffer[dataFieldWidth] = NULL;
			
			dataOut[i] = atoi(tempBuffer);

			// Can we update the system over serial?		
			if (serialLn != -1) {
				if (i == 0) sendData(serialLn, 'A', tempBuffer);       // Front heart rate
				else if (i == 1) sendData(serialLn, 'C', tempBuffer);  // Front cadence
				else if (i == 2) sendData(serialLn, 'E', tempBuffer);  // Front power
				else if (i == 3) sendData(serialLn, 'B', tempBuffer);  // Rear heart rate
				else if (i == 4) sendData(serialLn, 'D', tempBuffer);  // Rear cadence
				else if (i == 5) sendData(serialLn, 'F', tempBuffer);  // Rear power
			}
		}
#ifdef ANT_DEBUG_MESSAGES
		printf("ANT DATA IN: %03d,%03d,%03d|%03d,%03d,%03d\n", dataOut[0], dataOut[1], dataOut[2], dataOut[3], dataOut[4], dataOut[5]);
#endif
		return(0);
	}
	else {
		printf("Couldn't read any data from stdin for ANT.\n");
		return(-1);
	}
}