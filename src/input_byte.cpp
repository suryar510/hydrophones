#include <stdint.h>
#include <stdio.h>

#include "global.hpp"

void init_input() {
}

float data[num_channels];

const float* input() {
	while (getchar() != '\n');

	for (size_t i = 0; i < num_channels; ++i)
		data[i] = float(uint8_t(getchar())) / 256;

	return data;
}

