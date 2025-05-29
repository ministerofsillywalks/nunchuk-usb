#ifndef I2C_H
#define I2C_H


#include <hidapi/hidapi.h>

#define VID 0x04d8
#define PID 0x00dd

#define REPORT_ID 0x00

#define OPC_SET_PARAMS 0x10
#define OPC_I2C_WRITE 0x90
#define OPC_I2C_READ 0x91
#define OPC_I2C_GET 0x40

hid_device *dev_init(void);
int dev_close(hid_device *dev);

int i2c_set_clk(unsigned int clk_div, hid_device *dev);
int i2c_write(unsigned short data_len, unsigned char addr, unsigned char *data, hid_device *dev);
int i2c_read(unsigned short req_len, unsigned char addr, unsigned char *buf, hid_device *dev);
#endif
