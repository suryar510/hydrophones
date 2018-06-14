#include <stdint.h>
#include <stdio.h>

#include "global.hpp"

void init_input() {
}

uint16_t data[num_channels];

const uint16_t* input() {
	while (getchar() != '\n');

	for (size_t i = 0; i < num_channels; ++i)
		data[i] = getchar() << 4;

	return data;
}

