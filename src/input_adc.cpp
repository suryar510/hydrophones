#include <Arduino.h>

#include <stdint.h>

#include "global.hpp"

static uint64_t prev_time = 0;

void init_input() {
	prev_time = micros();
}

static float in[num_channels];

const float* input() {
	while (true) {
		const uint64_t time = micros();

		// continue at next tick
		if (time - prev_time >= 1000000 / sampling_rate) {
			prev_time = time;
			break;
		}

		// handle overflow
		if (time < prev_time) prev_time = time;
	}

	for (size_t i = 0; i < num_channels; ++i)
		in[i] = float(analogRead(A0 + i)) / 1024;

	return in;
}

