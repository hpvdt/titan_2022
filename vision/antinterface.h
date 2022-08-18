#ifndef ANT_INTERFACE_CFILE_H
#define ANT_INTERFACE_CFILE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <stdbool.h>

#include "serialComs.h"

extern const int numANTDataFields;

bool getANTDataPipedIn(int *dataOut, int serialLn); // Gets ANT data from stdin
bool getANTDataSerial(int *dataOut, int serialLn); // Get ANT data over serial

#endif