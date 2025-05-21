#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "arduino-serial-lib.h"
#define SERIALPORTDEBUG 1

int main (void) {
	char buf[100];	
	
	int fd = serialport_init("/dev/ttyUSB0", 9600);
	printf("Serial Port open on /dev/ttyUSB0\n");
	serialport_flush(fd);
	while (1) {
		serialport_read_until(fd, buf, '1', 100, 150);
		if (strcmp("51",buf)==0) {
			printf("Left Key");
			system("ydotool key 105:1 105:0");
			printf("%s\n", buf);
		}
		else if (strcmp("61",buf)==0) {
			printf("Right Key");
			system("ydotool key 106:1 106:0");
			printf("%s\n", buf);
		}
		else {
			printf("%s\n", buf);
		}
		//printf("%s", buf);
	}



}
