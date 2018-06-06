#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "global.hpp"

void init_process() {
}

char out[1024];

const char* process(const float* const in) {
	size_t out_idx = 0;
	out[0] = '\0';
	const size_t len_out = sizeof(out) / sizeof(char);

	for (size_t i = 0; i < num_channels; ++i) {
		if (i > 0) out_idx += snprintf(out + out_idx, len_out - out_idx, " ");
		out_idx += snprintf(out + out_idx, len_out - out_idx, "%.3f", in[i]);
	}
	out_idx += snprintf(out + out_idx, len_out - out_idx, "\n");

	return out;
}

