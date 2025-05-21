#include <hidapi/hidapi.h>

#define VID 0x04d8
#define PID 0x00dd

#define REPORT_ID 0x00

#define OPC_SET_PARAMS 0x10
#define OPC_I2C_WRITE 0x90
#define OPC_I2C_READ 0x91
#define OPC_I2C_GET 0x40

hid_device *dev_init(int clk_speed, int *res);

int i2c_write(short buf_len, char addr, char *buf);
char *i2c_read(short req_len, char addr, char *buf);
