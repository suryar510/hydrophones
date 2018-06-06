#include <stdint.h>
#include <stdio.h>

#include "global.hpp"

void init_input() {
}

int16_t data[num_channels];

const int16_t* input() {
	while (getchar() != '\n');

	for (size_t i = 0; i < num_channels; ++i)
		data[i] = int8_t(getchar()) * int16_t(256);

	return data;
}

