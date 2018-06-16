// this code simulates an input pinger with a large amount of gaussian noise
// (ie more noise than signal)

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "global.hpp"

static uint64_t frequency = 30000;
static int64_t coffset[] = {0, 7, -7, 5};
static float noise = 1.;

static constexpr const uint64_t sine_wave_size = 200;
static float sin_buf[sine_wave_size];

void init_input() {
	for (uint64_t i = 0; i < sine_wave_size; ++i) {
		const float angle = 2 * M_PI * i / sine_wave_size;
		sin_buf[i] = sin(angle);
	}
}

double randn(double mu, double sigma);

static int16_t v[num_channels][block_size];
static uint64_t iter = sampling_rate;

int16_t (*input())[block_size] {
	for (size_t block_idx = 0; block_idx < block_size; ++block_idx) {
		const uint64_t time = iter * 1000000 / sampling_rate; // in uc
		const uint64_t time_offset = time % 2000000;

		if (0 <= time_offset && time_offset < 4000) {
			for (uint8_t i = 0; i < num_channels; ++i) {
				float value = 2 + sin_buf[(frequency * (time + coffset[i]) % 1000000) * sine_wave_size / 1000000] + randn(0, noise);

				if (value > 4) value = 4;
				if (value < 0) value = 0;

				v[i][block_idx] = value / 4 * 4096;
			}
		}
		else {
			for (uint8_t i = 0; i < num_channels; ++i) {
				float value = 2 + randn(0, noise);
				if (value > 4) value = 4;
				if (value < 0) value = 0;
				v[i][block_idx] = value / 4 * 4096;
			}
		}

		++iter;
	}

	return v;
}

// sample from gaussian
// phoxis.org/2013/05/04/generating-random-numbers-from-normal-distribution-in-c
double randn(double mu, double sigma)
{
	double U1, U2, W, mult;
	static double X1, X2;
	static int call = 0;
 
	if (call == 1) {
		call = !call;
		return (mu + sigma * (double) X2);
	}
 
	do {
		U1 = -1 + ((double) rand() / RAND_MAX) * 2;
		U2 = -1 + ((double) rand() / RAND_MAX) * 2;
		W = pow (U1, 2) + pow (U2, 2);
	} while (W >= 1 || W == 0);
 
	mult = sqrt ((-2 * log (W)) / W);
	X1 = U1 * mult;
	X2 = U2 * mult;
 
	call = !call;
 
	return (mu + sigma * (double) X1);
}

