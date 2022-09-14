#ifndef SERIAL_COMS_H
#define SERIAL_COMS_H

#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <sys/ioctl.h>		//Used to check buffer

#include <stdbool.h>	// Used for testing to print or not

#include <string.h>
#include <stdlib.h>	// Used to grab int and float from char arrays
#include <errno.h>

#define BAUDRATE B115200 // Baudrate to be used (needs to be prefixed with 'B')

int openLine(char * line); // Used to open a specified serial line

void sendData(int line, char type, char* data); // Used to send data to microcontroller
// line - serial file descriptor of line to use
// type - type of data being sent
// data - data to send as a character string

void requestData(int line, char type, char* data); // Used to request data from 
// line - serial file descriptor of line to use
// type - type of data being srequested
// data - data buffer to write to
void requestDataInt(int line, char type, int* data);
void requestDataFloat(int line, char type, float* data);

void closeLine (int line); // Used to close a serial line

#endif