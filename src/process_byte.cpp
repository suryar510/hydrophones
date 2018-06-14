#include <stdint.h>

#include "global.hpp"

void init_process() {
}

char out[num_channels + 2];

const char* process(const uint16_t* const in) {
	for (size_t i = 0; i < num_channels; ++i) {
		out[i] = in[i] >> 4;
		if (out[i] == '\n') --out[i];
		if (out[i] == '\0') ++out[i];
	}
	out[num_channels] = '\n';
	out[num_channels + 1] = '\0';

	return out;
}

