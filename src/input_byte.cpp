#include <stdint.h>
#include <stdio.h>

#include "global.hpp"

void init_input() {
}

static int16_t data[num_channels][block_size];

int16_t (*input())[block_size] {
	while (getchar() != '\n');

	for (size_t block_idx = 0; block_idx < block_size; ++block_idx) {
		for (size_t i = 0; i < num_channels; ++i)
			data[i][block_idx] = getchar() << 4;
		getchar();
	}

	return data;
}

