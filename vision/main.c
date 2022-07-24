#include "main.h"

int main() {
   startOverlay(false);  // Camera off
   
   time_t t,t2; // Realtime marks
   time(&t); // Get start time
   clock_t c = clock(); // Get start processor time
   
   // Open serial line
   if (useSerial == true) {
      serialLine = openLine("/dev/serial0");
   }
   else {
      printf("NOT USING SERIAL DATA. JUST A TEST\n");
   }
   if (useANT == true) {
      printf("EXPECTING ANT DATA PIPED IN\n");
   }
   else {
      printf("NOT USING ANT DATA. JUST MAKING STUFF UP.\n");
   }
   
   for (int i = 0; i < numberFrames; i++) {    
      
      // ANT data
      // Checks for data, then if it is likely valid before reading
      if (useANT == true) {
         if (fgets(ANTBuffer, bufferLength, stdin) != NULL) {

            // Check if it is text and skip
            // Numeric data should start with a number
            if ((ANTBuffer[0] < '0') || (ANTBuffer[0] > '9')) {
               continue;
            }
            
            // Data is always set in a set length
            for (int i = 0; i < numDataFields; i++) {
               int temp = 0;
               
               // TODO: Make a loop based on dataFieldWidth?
               char tempBuffer[dataFieldWidth];
               tempBuffer[0] = ANTBuffer[(i * dataFieldWidth)];
               tempBuffer[1] = ANTBuffer[(i * dataFieldWidth) + 1];
               tempBuffer[2] = ANTBuffer[(i * dataFieldWidth) + 2];
               
               ANTData[i] = atoi(tempBuffer);
               
               // Can we update the system over serial?
               if (useSerial == true) {
                  // Prepare to send new data over serial
                  sprintf("%d",tempBuffer , ANTData[i]);
                  
                  if (i == 0) sendData(serialLine, 'A', tempBuffer);       // Front heart rate
                  else if (i == 1) sendData(serialLine, 'C', tempBuffer);  // Front cadence
                  else if (i == 2) sendData(serialLine, 'E', tempBuffer);  // Front power
                  else if (i == 3) sendData(serialLine, 'B', tempBuffer);  // Rear heart rate
                  else if (i == 4) sendData(serialLine, 'D', tempBuffer);  // Rear cadence
                  else if (i == 5) sendData(serialLine, 'F', tempBuffer);  // Rear power
               }
            }
            heartRate = ANTData[0];
            cadence = ANTData[1];
            power = ANTData[2];
         }
      }
      
      
      // Serial Data
      if (useSerial == true) {
         // Request data
         // Actual bike running
         if (useANT == false) {
            requestDataInt(serialLine, 'a', &heartRate); 
            requestDataInt(serialLine, 'c', &cadence);
            requestDataInt(serialLine, 'e', &power);
         }
         requestDataFloat(serialLine, 's', &speed); 
         requestDataInt(serialLine, 'q', &rotations);
         distance = rotations * CIRCUMFERENCE;
         
         requestDataFloat(serialLine, 't', &temperature); 
         requestDataFloat(serialLine, 'h', &humidity); 
         requestDataInt(serialLine, 'i', &battery); 
         
         updateOverlay(speed, distance, i, cadence, heartRate, temperature, humidity, battery);
      }
      else {
         // Just test the overlay
         if (useANT == true) updateOverlayTest(i, power, cadence, heartRate);
         else updateOverlayTest(i, 75, 48, 97);
         
      }
   }
   c = clock() - c; // Get run time
   time(&t2); // Get end time
   
   closeOverlay();
   
   float secondsElapsed = ((float) c)/CLOCKS_PER_SEC;
   
   printf("\nIt took %f processor seconds, %.f realtime seconds for %d frames.\n", secondsElapsed, difftime(t2,t), numberFrames);
   return 0;
}

