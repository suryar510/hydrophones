#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "global.hpp"

void init_process() {
}

char out[4096];

const char* process(int16_t (* const in)[block_size]) {
	size_t out_idx = 0;
	out[0] = '\0';
	const size_t len_out = sizeof(out) / sizeof(char);

	for (size_t block_idx = 0; block_idx < block_size; ++block_idx) {
		for (size_t i = 0; i < num_channels; ++i) {
			if (i > 0) out_idx += snprintf(out + out_idx, len_out - out_idx, " ");
			out_idx += snprintf(out + out_idx, len_out - out_idx, "%" PRIu32, in[i][block_idx]);
		}
		out_idx += snprintf(out + out_idx, len_out - out_idx, "\n");
	}

	return out;
}

