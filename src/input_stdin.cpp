#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "global.hpp"

void init_input() {
}

int16_t data[num_channels];

const int16_t* input() {
	while (true) {
		while (getchar() != '\n');

		bool failed = false;
		for (size_t i = 0; i < num_channels; ++i)
			failed = failed || scanf(" %" SCNd16, &data[i]) != 1;

		if (!failed) return data;
	}
}

