#include <stdio.h>

#include "nunchuk.h"

int nchuk_init() {
	printf("Hello World\n");
	return 1;
}

int nchuk_update(struct nchuk_state *state) {
	state->c_but = 1;
}
