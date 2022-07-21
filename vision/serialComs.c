#include "serialComs.h"

int openLine(char * line) {
	// Open serial line
	int outputLine = -1;
	
	//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes: O_RDWR - Open for reading and writing.
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	outputLine = open(line, O_RDWR | O_NOCTTY);		//Open in blocking read/write mode
	if (outputLine == -1) {
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
		return outputLine;
	}
	//printf("Successfully opened serial on %s.\n", line);
	
	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	struct termios options;
	tcgetattr(outputLine, &options);
	options.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	options.c_cc[VMIN] = 0;	// Minimum characters per read operation
	options.c_cc[VTIME] = 2;
	
	tcflush(outputLine, TCIOFLUSH);
	tcsetattr(outputLine, TCSANOW, &options);
	
	return outputLine;
}

void sendData(int line, char type, char* data){
	if (line == -1) return; // Exit if no line set up
	tcflush(line, TCOFLUSH);
	unsigned char length = strlen(data) + 31;
	
	char buffer[30];
	sprintf(buffer, "%c%c%s", type, length, data);
	//printf("Sending data (%s)...", buffer);
	int sentBytes = write(line, buffer, strlen(buffer));
	tcdrain(line);
	//printf("done sending %d bytes!\n", sentBytes);
}

void requestData(int line, char type, char* data) {
	// Requests data over serial, stores it to a char array that is passed
	// into the function and refernced.
	
	//printf("Starting request...");
	if (line == -1) return; // Return on no line
	tcflush(line, TCIOFLUSH);
	
	// Send out request
	int count = write(line, &type, 1);
	if (count < 0){
		//printf("UART TX error for request\n");
		return;
	}
	tcdrain(line); // Wait for request to leave output buffer
	//printf("done!. Sent %c.\n", type);
	
	// Get back length of incoming data
	unsigned char rxLength = 0;
	read(line, &rxLength, 1);
	rxLength -= 31;
	
	int bytes = 0;
	ioctl(line, FIONREAD, &bytes);
	//printf("Length of return is %d, bytes waiting %d.\n", (int)rxLength, bytes);
	
	// Read in data to the buffer passed into the function byte by byte
	read(line, data, rxLength);
	data[rxLength] = '\0'; // Terminate the string
	
	//printf("Recieved %s, (length of %d).\n", data, rxLength);
}
void requestDataInt(int line, char type, int* data){
	char temp[10]; // Temporary string
	requestData(line, type, temp);
	
	sscanf(temp, "%d", data); // Scans recieved message for integer
}
void requestDataFloat(int line, char type, float* data){
	char temp[10]; // Temporary string
	requestData(line, type, temp);
	
	sscanf(temp, "%f", data); // Scans recieved message for float
}

void closeLine (int line) {
	if (line == -1) return;
	close(line);
}
