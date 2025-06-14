#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/uinput.h>
#include <signal.h>
#include <fcntl.h>
#include <hidapi/hidapi.h>

#include "nunchuk.h"
#include "i2c.h"


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
	int res;
	int failcount = 0;
	int up_bind, down_bind, left_bind, right_bind, c_bind, z_bind, accel_x_bind, accel_y_bind, accel_z_bind;	
	signal(SIGINT, interrupt); 
	struct nchuk *nchuk = nchuk_init();

	*cmdopts = getopt(argc, argv, "m");
	if (*cmdopts == '?') printf("Invalid option, assuming verbose mode off\n");
	
	struct uinput_setup usetup;
	int fd_dev = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fd_dev == -1) {
		fprintf(stderr, "Could not open /dev/uinput! Error no. %d. Exiting\n", errno);
		return 1;
	}
	if (!(*cmdopts == 'm')) {
		ioctl(fd_dev, UI_SET_EVBIT, EV_KEY);
		ioctl(fd_dev, UI_SET_KEYBIT, KEY_LEFT);
		ioctl(fd_dev, UI_SET_KEYBIT, KEY_RIGHT);
		ioctl(fd_dev, UI_SET_KEYBIT, KEY_UP);
		ioctl(fd_dev, UI_SET_KEYBIT, KEY_DOWN);

		ioctl(fd_dev, UI_SET_KEYBIT, KEY_Z);
		ioctl(fd_dev, UI_SET_KEYBIT, KEY_X);
		ioctl(fd_dev, UI_SET_KEYBIT, KEY_C);

		memset(&usetup, 0, sizeof(usetup));
		usetup.id.bustype = BUS_USB;
		usetup.id.vendor = 0xbbbb;
		usetup.id.product = 0xbbbb;
		strcpy(usetup.name, "Wii Nunchuk");
		
		ioctl(fd_dev, UI_DEV_SETUP, &usetup);
		ioctl(fd_dev, UI_DEV_CREATE);

		while (!_break && failcount < 3) {
			failcount += nchuk_update(nchuk);

			if (nchuk->state.joy_x < 65) {
				emit(fd_dev, EV_KEY, KEY_LEFT, 1);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}
			else {
				emit(fd_dev, EV_KEY, KEY_LEFT, 0);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}	
			
			if (nchuk->state.joy_x > 200) {
				emit(fd_dev, EV_KEY, KEY_RIGHT, 1);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}
			else {
				emit(fd_dev, EV_KEY, KEY_RIGHT, 0);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}
			
			if (nchuk->state.joy_y > 200) {
				emit(fd_dev, EV_KEY, KEY_UP, 1);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}
			else {
				emit(fd_dev, EV_KEY, KEY_UP, 0);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}		

			if (nchuk->state.joy_y < 65) {
				emit(fd_dev, EV_KEY, KEY_DOWN, 1);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}
			else {
				emit(fd_dev, EV_KEY, KEY_DOWN, 0);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}

			if (nchuk->state.c_but && !nchuk->state.z_but) {
				emit(fd_dev, EV_KEY, KEY_Z, 1);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}
			else {
				emit(fd_dev, EV_KEY, KEY_Z, 0);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}

			if (nchuk->state.z_but && !nchuk->state.c_but) {
				emit(fd_dev, EV_KEY, KEY_X, 1);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}
			else {
				emit(fd_dev, EV_KEY, KEY_X, 0);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}

			if (nchuk->state.z_but == 1 && nchuk->state.c_but == 1) {
				emit(fd_dev, EV_KEY, KEY_C, 1);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}
			else {
				emit(fd_dev, EV_KEY, KEY_C, 0);
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}

		}
	}

	else if (*cmdopts == 'm') {
		ioctl(fd_dev, UI_SET_EVBIT, EV_KEY);
		ioctl(fd_dev, UI_SET_KEYBIT, BTN_LEFT);
		ioctl(fd_dev, UI_SET_KEYBIT, BTN_RIGHT);

		ioctl(fd_dev, UI_SET_EVBIT, EV_REL);
		ioctl(fd_dev, UI_SET_RELBIT, REL_X);
		ioctl(fd_dev, UI_SET_RELBIT, REL_Y);
		ioctl(fd_dev, UI_SET_RELBIT, REL_WHEEL);

		memset(&usetup, 0, sizeof(usetup));
		usetup.id.bustype = BUS_USB;
		usetup.id.vendor = 0xbbbb;
		usetup.id.product = 0xbbbb;
		strcpy(usetup.name, "Wii Nunchuk");
		
		ioctl(fd_dev, UI_DEV_SETUP, &usetup);
		ioctl(fd_dev, UI_DEV_CREATE);


		while (!_break && failcount < 3) {
			failcount += nchuk_update(nchuk);

			if(!(nchuk->state.c_but && nchuk->state.z_but)) {
				if(nchuk->state.joy_x > 160) {
					emit(fd_dev, EV_REL, REL_X, (255 - nchuk->state.joy_x)/2);
				}
				else if(nchuk->state.joy_x < 100) {
					emit(fd_dev, EV_REL, REL_X, (nchuk->state.joy_x - 100)/2);
				}
				
				if(nchuk->state.joy_y > 160) {
					emit(fd_dev, EV_REL, REL_Y, (nchuk->state.joy_y - 255)/2);
				}
				else if(nchuk->state.joy_y < 100) {
					emit(fd_dev, EV_REL, REL_Y, (100 - nchuk->state.joy_y)/2);
				}
				
				if(nchuk->state.c_but) { 
					emit(fd_dev, EV_KEY, BTN_LEFT, 1);
				}
				else {
					emit(fd_dev, EV_KEY, BTN_LEFT, 0);
				}

				if(nchuk->state.z_but) { 
					emit(fd_dev, EV_KEY, BTN_RIGHT, 1);
				}
				else {
					emit(fd_dev, EV_KEY, BTN_RIGHT, 0);
				}
				
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}
			else {
				if(nchuk->state.joy_y > 160) {
					emit(fd_dev, EV_REL, REL_WHEEL, (nchuk->state.joy_y - 255)/10);
				}
				else if(nchuk->state.joy_y < 100) {
					emit(fd_dev, EV_REL, REL_WHEEL, (100 - nchuk->state.joy_y)/10);
				}
			
				emit(fd_dev, EV_SYN, SYN_REPORT, 0);
			}

		}
	}

	if (failcount >= 3) printf("HID operations failed 3 times. Exiting.\n");	
	else printf("Loop broken! Destroying device and exiting\n");
	ioctl(fd_dev, UI_DEV_DESTROY);
	close(fd_dev);
	
	return 0;

}
