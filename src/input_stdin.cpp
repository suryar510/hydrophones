#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "global.hpp"

void init_input() {
}

int16_t data[num_channels][block_size];

int16_t (*input())[block_size] {
	while (true) {
		while (getchar() != '\n');

		bool failed = false;
		for (size_t block_idx = 0; block_idx < block_size; ++block_idx) {
			for (size_t i = 0; i < num_channels; ++i)
				failed = failed || scanf(" %" SCNu16, &data[i][block_idx]) != 1;
			getchar();
		}

		if (!failed) return data;
	}
}

