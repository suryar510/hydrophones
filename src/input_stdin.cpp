#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "global.hpp"

void init_input() {
}

float data[num_channels];

const float* input() {
	while (true) {
		while (getchar() != '\n');

		bool failed = false;
		for (size_t i = 0; i < num_channels; ++i)
			failed = failed || scanf(" %f", &data[i]) != 1;

		if (!failed) return data;
	}
}

