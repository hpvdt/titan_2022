#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "serialComs.c"
#include <time.h>

int serialLine = 0;
char bufferIn[10];
char bufferOut[10];

int main() {
	serialLine = openLine("/dev/serial0");
	
	if (serialLine == -1) {
		return (1);
	}
	
	bufferOut[0] = '3';
	sendData(serialLine, 'Y', bufferOut);
	printf("Sent 'Y' to be %s.\n", bufferOut);

	requestData(serialLine, 'y', bufferIn);
	printf("Requested 'y', recieved: %s\n", bufferIn);
	
	closeLine(serialLine);
	printf("Closed line.\n");
}