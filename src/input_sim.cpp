// this code simulates an input pinger with a large amount of gaussian noise
// (ie more noise than signal)

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "global.hpp"

static float frequency = 30000;
static float coffset[] = {0, 7e-6, -7e-6, 4.7e-6};
static float noise = 1.;

void init_input() {
}

double randn(double mu, double sigma);

static int16_t v[num_channels];
static uint64_t iter = 0;

const int16_t* input() {
	const float time = float(iter) / sampling_rate;
	const float time_offset = fmod(time, 2.);

	const float scaling = 32768 / 4;
	if (0 < time_offset && time_offset < .004) {
		for (uint8_t i = 0; i < num_channels; ++i) {
			float value = sin(2 * M_PI * frequency * (time + coffset[i])) + randn(0, noise);

			if (value > 2) value = 2;
			if (value < -2) value = -2;

			v[i] = value * scaling;
		}
	}
	else {
		for (uint8_t i = 0; i < num_channels; ++i)
			v[i] = randn(0, noise) * scaling;
	}

	++iter;
	if (iter >= 2 * sampling_rate) iter = 0;

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

