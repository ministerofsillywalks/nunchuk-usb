#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "i2c.h"

hid_device *dev_init(void) {
	hid_device *dev;
	int res = 0;
	if (res = hid_init() == -1) {
		fprintf(stderr, "Failure to initialize hidapi\n");
		return NULL;
	}
	dev = hid_open(VID,PID,NULL);
	if (!dev) {
		fprintf(stderr, "Failure to open device, failure reason: %ls\n", hid_error(dev));
		return NULL;
	}
	printf("Successfully connected to hid device!\n");
	unsigned char buf[100];
	/*if (hid_get_report_descriptor(dev, buf, 100) > 0){
       		printf("%s\n", buf);	
	}
	else {
		printf("No Bytes Copied");
	}*/

	return dev;
}

int i2c_set_clk(unsigned int clk_div, hid_device *dev) { 
	unsigned char buf[65] = {0};
	buf[0] = 0x0;
	int res = 0;
	memset(buf, 0, sizeof(buf));	
	
	//setting the buffer for the Status/Set Parameters Command
	buf[1] = OPC_SET_PARAMS;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = 0x20;
	buf[5] = clk_div;
	
	//write the new speed to the device!
	res = hid_write(dev, buf, sizeof(buf));
	if(res == -1) {
		fprintf(stderr, "Error writing to device. Error reason %ls.\n", hid_error(dev));
		return -1;
	}

	//read the response.
	res = hid_read(dev, buf, 65);
	if(res == -1) {
		fprintf(stderr, "Error reading to device. Error reason %ls.\n", hid_error(dev));
		return -1;
	}
	if(buf[3] == 0x21) {
		fprintf(stderr, "Error setting clock speed.\n");
		return 0x21;
	}

	return res;
}

int i2c_write(unsigned short data_len, unsigned char addr, unsigned char *data, hid_device *dev) {
	unsigned char buf[65];
	memset(buf, 0, 65);
	int res = 0;
	
	int data_len_low = data_len & 0x00ff;
        int data_len_high = (data_len & 0xff00) >> 8;
	addr <<= 1;

	//set the buffer
	buf[1] = OPC_I2C_WRITE;
	buf[2] = data_len_low;
	buf[3] = data_len_high;
	buf[4] = addr;
	
	if(data_len <= 60) {
		memcpy(buf+5, data, data_len);
	}
	else printf("Data is too long. Please only write data less than 60 bytes.\n");

	//write to the device
	res = hid_write(dev, buf, 65);
	if (res != 65 && res != -1) {
		printf("Warning: only %d bytes were written to the device\n", res);
	}
	else if (res == -1) {
		fprintf(stderr, "Error: No data was written. Error reason: %ls\n", hid_error(dev));
		return res;
	}
	//else printf("Successfully wrote to device\n");	

	//await response
	hid_read(dev, buf, 65);
	if (res == -1) {
	       fprintf(stderr, "Error: no data was read. Error reason: %ls\n", hid_error(dev));
	       return res;
	}
	
	//check response
	if (buf[1] != 0) {
		printf("Error executing command: i2c engine busy\n");
	}

	
	return res;
}

int i2c_read(unsigned short req_len, unsigned char addr, unsigned char *buf, hid_device *dev) {
	unsigned char cmd_buf[65];
	memset(cmd_buf, 0, 65);
	int res = 0;

	int data_len_low = req_len & 0x00ff;
        int data_len_high = (req_len & 0xff00) >> 8;
	addr <<= 1;
	addr++;
	
	//First: ask the mcp to retrieve the i2c data from the i2c device
	cmd_buf[1] = OPC_I2C_READ;
	cmd_buf[2] = data_len_low;
	cmd_buf[3] = data_len_high;
	cmd_buf[4] = addr;

	//write the read request to the device
	res = hid_write(dev, cmd_buf, 65);
	if (res != 65 && res != -1) {
		printf("Warning: only %d bytes were written to the device\n", res);
	}
	else if (res == -1) {
		fprintf(stderr, "Error: No data was written. Error reason: %ls\n", hid_error(dev));
		return res;
	}
	
	//read from the device
	hid_read(dev, cmd_buf, 65);
	if (res == -1) {
	       fprintf(stderr, "Error: no data was read. Error reason: %ls\n", hid_error(dev));
	       return res;
	}
	
	//check response
	if (cmd_buf[1] != 0) {
		fprintf(stderr, "Error executing command: i2c engine busy\n");
	}
	
	//next, ask the mcp to send the data it recieved from the i2c device
	memset(cmd_buf, 0, 65);
	cmd_buf[1] = OPC_I2C_GET;

	res = hid_write(dev, cmd_buf, 65);
	if (res != 65 && res != -1) {
		printf("Warning: only %d bytes were written to the device\n", res);
	}
	else if (res == -1) {
		fprintf(stderr, "Error: No data was written. Error reason: %ls\n", hid_error(dev));
		return res;
	}
	
	//read from the device
	res = hid_read(dev, cmd_buf, 65);
	if (res == -1) {
	       fprintf(stderr, "Error: no data was read. Error reason: %ls\n", hid_error(dev));
	       return res;
	}
	
	//check response
	if (cmd_buf[1] == 0x41) {
		fprintf(stderr, "Error reading data from the i2c device\n");
		return 0x41;
	}

	//printf("%d bytes read from the i2c device.\n", cmd_buf[3]);
	memcpy(buf, cmd_buf+4, cmd_buf[3]);

	return 0;
}

int dev_close(hid_device *dev) {
	hid_close(dev);
	hid_exit();
}
