#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "global.hpp"

void init_input() {
}

uint16_t data[num_channels];

const uint16_t* input() {
	while (true) {
		while (getchar() != '\n');

		bool failed = false;
		for (size_t i = 0; i < num_channels; ++i)
			failed = failed || scanf(" %" SCNu16, &data[i]) != 1;

		if (!failed) return data;
	}
}

