#include <stdint.h>

#include "global.hpp"

void init_process() {
}

char out[block_size * (num_channels + 2)];

const char* process(int16_t (* const in)[block_size]) {
	char* ptr = out;
	for (size_t block_idx = 0; block_idx < block_size; ++block_idx) {
		for (size_t i = 0; i < num_channels; ++i) {
			ptr[i] = in[i][block_idx] >> 4;
			if (ptr[i] == '\n') --out[i];
			if (ptr[i] == '\0') ++out[i];
		}
		ptr[num_channels] = '\n';

		ptr += num_channels + 1;
	}
	ptr[0] = '\0';

	return out;
}

