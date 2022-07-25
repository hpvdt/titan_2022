#ifndef TIME_TRIAL_HEADER
#define TIME_TRIAL_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void startTrial();

u_int64_t endTrial(char * nameOfTrial);
// Prints name of trial
u_int64_t endTrialIgnore(char * nameOfTrial, u_int64_t ignoreLimit);
// If the delta in real time is less than 'ignoreLimit' (ms) then no message is printed
// Returns real time passed in ms as u_int64_t

#endif