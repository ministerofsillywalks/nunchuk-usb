#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "nunchuk.h"

const int addr = 0x52;

struct nchuk_state _nchuk_analyze(char data[6]) {
	struct nchuk_state state = {0};
	
	state.joy_x = data[0];
	state.joy_y = data[1];
	
	state.accel_x = data[2] << 2;
	state.accel_y = data[3] << 2;
	state.accel_z = data[4] << 2;
	
	state.c_but = !((data[5] & 0b00000010) >> 1);
	state.z_but = !(data[5] & 0b00000001);
	
	state.accel_x += ((data[5] & 0b11000000) >> 6);
	state.accel_y += ((data[5] & 0b00110000) >> 4);
	state.accel_z += ((data[5] & 0b00001100) >> 2);

	return state;
}

struct nchuk *nchuk_init(void) {
	char handshake_1[2] = {0xf0, 0x55};
	char handshake_2[2] = {0xfb, 0x00};
	char buf[6] = {0};
	struct nchuk *nchuk = malloc(sizeof(nchuk));

	memset(nchuk, 0, sizeof(nchuk));

	nchuk->dev = dev_init();

	if(!nchuk->dev) {
		fprintf(stderr, "Error initializing nunchuk!");
		free(nchuk);
		return NULL;
	}

	i2c_set_clk(120, nchuk->dev);
	usleep(20000);
	i2c_write(2, addr, handshake_1, nchuk->dev);
	usleep(20000);
	i2c_write(2, addr, handshake_2, nchuk->dev);
	usleep(20000);
	i2c_read(6, addr, buf, nchuk->dev);

	nchuk->state = _nchuk_analyze(buf);

	return nchuk;
}

int nchuk_update(struct nchuk *nchuk) {
	char req_update[2] = {0x0};
	char buf[6];
	int res;

	res = i2c_write(1, addr, req_update, nchuk->dev);
	if(res == -1) return 1;
	usleep(20000);
	res = i2c_read(6, addr, buf, nchuk->dev);
	if(res == -1) return 1;
	usleep(20000);

	nchuk->state = _nchuk_analyze(buf);
#ifdef DEBUGLOG	
	printf("\rJoyX: %d, JoyY: %d, AccelX: %d, AccelY: %d, AccelZ: %d, Cbut: %d, Zbut: %d", nchuk->state.joy_x, nchuk->state.joy_y, nchuk->state.accel_x, nchuk->state.accel_y, nchuk->state.accel_z, nchuk->state.c_but, nchuk->state.z_but);
	fflush(stdout);
#endif
	return 0;
}

int nchuk_destroy(struct nchuk *nchuk) {
	dev_close(nchuk->dev);
	free(nchuk);
	return 0;
}
