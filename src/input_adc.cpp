#include <Arduino.h>

#include <stdint.h>

#include "global.hpp"

void init_input() {
}

float in[num_channels];

const float* input() {
	for (size_t i = 0; i < num_channels; ++i)
		in[i] = float(analogRead(A0 + i)) / 1024;

	return in;
}

