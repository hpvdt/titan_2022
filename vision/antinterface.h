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

bool getANTData(int *dataOut, int serialLn);

#endif