#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/uinput.h>
#include <signal.h>
#include <fcntl.h>

#include "arduino-serial-lib.h"

int _break = 0;
int cmdopts[0];

void interrupt(int signum) {
	_break = 1;
}


void emit(int fd, int type, int code, int val) {
	struct input_event ie;

	ie.type = type;
	ie.code = code;
	ie.value = val;
	ie.time.tv_sec = 0;
	ie.time.tv_usec = 0;
	
	if (write(fd, &ie, sizeof(ie)) == -1) {
		fprintf(stderr, "Could not write input, error number %d", errno);
	}	
}

int main (int argc, char **argv) {
	
	*cmdopts = getopt(argc, argv, "v");
	if (*cmdopts == '?') printf("Invalid option, assuming verbose mode off\n");
	
	signal(SIGINT, interrupt); 
	struct uinput_setup usetup;
	int fd_dev = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fd_dev == -1) {
		fprintf(stderr, "Could not open /dev/uinput! Error no. %d. Exiting\n", errno);
		return 1;
	}
	
	ioctl(fd_dev, UI_SET_EVBIT, EV_KEY);
	ioctl(fd_dev, UI_SET_KEYBIT, KEY_LEFT);
	ioctl(fd_dev, UI_SET_KEYBIT, KEY_RIGHT);

	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = 0xbbbb;
	usetup.id.product = 0xbbbb;
	strcpy(usetup.name, "Wii Nunchuk");
	
	ioctl(fd_dev, UI_DEV_SETUP, &usetup);
	ioctl(fd_dev, UI_DEV_CREATE);

	char buf[100];	
	
	int fd_serial = serialport_init("/dev/ttyUSB0", 9600);
	if (fd_serial == -1) {
		printf("Error initializing serial port! Exiting.");
		return 1;
	}

	printf("Serial Port open on /dev/ttyUSB0\n");
	serialport_flush(fd_serial);
	
	while (!_break) {
		serialport_read_until(fd_serial, buf, '1', 100, 150);
		if (strcmp("51",buf)==0) {
			printf("Left Key");
			emit(fd_dev, EV_KEY, KEY_LEFT, 1);
			emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			emit(fd_dev, EV_KEY, KEY_LEFT, 0);
			emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			if (*cmdopts == 'v') printf("%s\n", buf);
		}
		else if (strcmp("61",buf)==0) {
			printf("Right Key");
			emit(fd_dev, EV_KEY, KEY_RIGHT, 1);
			emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			emit(fd_dev, EV_KEY, KEY_RIGHT, 0);
			emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			if (*cmdopts == 'v') printf("%s\n", buf);
		}
		else {
			if (*cmdopts == 'v') printf("%s\n", buf);
		}
	}
	
	printf("Loop broken! Destroying device and exiting\n");
	ioctl(fd_dev, UI_DEV_DESTROY);
	close(fd_dev);
	
	return 0;

}
