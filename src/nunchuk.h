#ifndef NUNCHUK_H
#define NUNCHUK_H

#include <stdbool.h>
#include "i2c.h"

#define NCHUK_BUF_SIZE 6

struct nchuk_state {
	unsigned char joy_x;
	unsigned char joy_y;
	unsigned short accel_x:10;
	unsigned short accel_y:10;
	unsigned short accel_z:10;
	bool c_but;
	bool z_but;
};

struct nchuk {
	hid_device *dev;
	struct nchuk_state state;
};

struct nchuk *nchuk_init(void);
int nchuk_update(struct nchuk *nchuk);
int nchuk_destroy(struct nchuk *nchuk);

#endif
