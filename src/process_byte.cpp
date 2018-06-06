#include <stdint.h>

#include "global.hpp"

void init_process() {
}

char out[num_channels + 1];

const char* process(const int16_t* const in) {
	for (size_t i = 0; i < num_channels; ++i) {
		out[i] = char(int8_t(in[i] / 256));
		if (out[i] == '\n') --out[i];
	}
	out[num_channels] = '\n';

	return out;
}

