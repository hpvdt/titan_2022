#include <stdio.h>
#include <stdlib.h>

const int dataFieldWidth = 4; // Include delimiter!
const int numDataFields = 6;
#define bufferLength 60

char inbuffer[bufferLength];
int data[6];

int main () {
    printf("Enter data to be doubled:\nxxx,xxx,xxx,xxx,xxx,xxx\n");

    while (1) {
        fgets(inbuffer, bufferLength, stdin);

        // Check if it is text and skip
        if ((inbuffer[0] < '0') || (inbuffer[0] > '9')) {
            continue;
        }

        

        for (int i = 0; i < numDataFields; i++) {
            int temp = 0;
            char tempBuffer[3];
            tempBuffer[0] = inbuffer[(i * dataFieldWidth)];
            tempBuffer[1] = inbuffer[(i * dataFieldWidth) + 1];
            tempBuffer[2] = inbuffer[(i * dataFieldWidth) + 2];
            
            temp = atoi(tempBuffer); // Need to include delimiter so numbers that follow can be properly read 
            // (probably has to do with commas being used as decimal points in other places)
            data[i] = temp * 2;

            if (i == (numDataFields - 1)) printf("%03d", temp);
            else printf("%03d,", temp);
        }
        printf("  |  ");
        for (int i = 0; i < numDataFields; i++) {
            
            if (i == (numDataFields - 1)) printf("%03d", data[i]);
            else printf("%03d,", data[i]);
        }
        printf("\n");
    }
}