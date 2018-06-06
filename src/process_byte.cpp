#include <stdint.h>

#include "global.hpp"

void init_process() {
}

char out[num_channels + 2];

const char* process(const float* const in) {
	for (size_t i = 0; i < num_channels; ++i) {
		out[i] = uint8_t(in[i] * 255);
		if (out[i] == '\n') --out[i];
	}
	out[num_channels] = '\n';
	out[num_channels + 1] = '\0';

	return out;
}

