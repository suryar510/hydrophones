#include <Arduino.h>

#include <stdint.h>

#include "global.hpp"

static uint64_t prev_tick = 0;

void init_input() {
}

static float in[num_channels];

const float* input() {
	while (true) {
		// TODO: handle overflow
		//       currently incurs a slight inaccuracy after 70 min
		//       but that should only affect the next window
		const uint64_t time = micros();
		const uint64_t tick = time * sampling_rate / 1000000;

		// continue at next tick
		if (tick != prev_tick) {
			prev_tick = tick;
			break;
		}
	}

	for (size_t i = 0; i < num_channels; ++i)
		in[i] = float(analogRead(A0 + i)) / 1024;

	return in;
}

