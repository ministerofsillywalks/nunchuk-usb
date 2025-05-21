#define NCHUK_BUF_SIZE 6

struct nchuk_state {
	char jstick_x;
	char jstick_y;
	char accel_x;
	char accel_y;
	char accel_z;
	bool c_but;
	bool z_but;
};

int nchuk_init();
int nchuk_update(struct nchuk_state *state);
	
