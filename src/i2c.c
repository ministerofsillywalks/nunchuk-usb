#include <stdio.h>

#include "i2c.h"

hid_device *dev_init(int clkspeed, int *res) {
	hid_device *handle;
	if (*res = hid_init() == -1) {
		fprintf(stderr, "Failure to initialize hidapi\n");
	}
	handle = hid_open(VID,PID,NULL);
	if (!handle) {
		fprintf(stderr, "Failure to open device, failure reason: %s\n", hid_error(handle));
		return NULL;
	}
	printf("Successfully connected to hid device!\n");
	unsigned char buf[100];
	//if (hid_get_report_descriptor(handle, buf, 100) > 0){
       	//	printf("%s\n", buf);	
	//}
	//else {
	//	printf("No Bytes Copied");
	//}


	return handle;
}


