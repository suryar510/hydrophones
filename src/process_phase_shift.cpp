#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "global.hpp"

static constexpr const float alpha = .99;
static constexpr const float window = 1996000; // in us
static constexpr const float threshold = .0;
static constexpr const bool normalize = false;

static const uint64_t frequencies[] = {
	25000,
	30000,
	35000,
	40000,
};
static const int8_t  num_frequencies = sizeof(frequencies) / sizeof(uint64_t);
static const uint64_t gcd = 5000;
static const uint64_t period = sampling_rate / gcd;

static float avg_amplitude[num_channels];

static constexpr const uint64_t sine_wave_size = 200;
static float sin_buf[sine_wave_size];
static float cos_buf[sine_wave_size];

void init_process() {
	for (uint8_t i = 0; i < num_frequencies; ++i)
		avg_amplitude[i] = 1;

	for (uint64_t i = 0; i < sine_wave_size; ++i) {
		const float angle = 2 * M_PI * i / sine_wave_size;
		sin_buf[i] = sin(angle);
		cos_buf[i] = cos(angle);
	}
}

static float max_amplitude[num_frequencies] = {}; // the highest recently observed amplitude
static uint64_t max_time[num_frequencies] = {}; // the time of the highest observed amplitude
static float max_phase[num_frequencies][num_channels] = {}; // the phase of each channel when the highest amplitude was observed

static float real[num_frequencies][num_channels] = {};
static float imag[num_frequencies][num_channels] = {};

static char out[1024];
static const size_t len_out = sizeof(out) / sizeof(char);
static uint64_t iter = 0;

static constexpr const float beta = 1 - alpha;

const char* process(const float* const in) {
	size_t out_idx = 0;
	out[0] = '\0';

	const uint64_t time = iter * 1000000 / sampling_rate; // in uc

	// dft for only the relavent frequencies
	for (uint8_t f = 0; f < num_frequencies; ++f) {
		const uint64_t microrevs = frequencies[f] * time % 1000000; // angle = 2pi * microrevs / 1000000
		const uint64_t wave_idx = microrevs * sine_wave_size / 1000000;
		for (uint8_t c = 0; c < num_channels; ++c) {
			real[f][c] = real[f][c] * alpha + in[c] * sin_buf[wave_idx] * beta;
			imag[f][c] = imag[f][c] * alpha - in[c] * cos_buf[wave_idx] * beta;
		}
	}

	// compute averages for each channel
	if (normalize) {
		for (uint8_t f = 0; f < num_frequencies; ++f) {
			avg_amplitude[f] = avg_amplitude[f] * alpha;
			for (uint8_t c = 0; c < num_channels; ++c) {
				const float amplitude = sqrt(real[f][c] * real[f][c] + imag[f][c] * imag[f][c]);
				avg_amplitude[c] += amplitude / num_channels * (1 - alpha);
			}
		}
	}

	// find time for which the minimum amplitude is highest
	if (time > window) {
		for (uint8_t f = 0; f < num_frequencies; ++f) {
			// handle overflow
			// should happen rarely
			if (time < max_time[f]) max_time[f] = 0;

			float min_amplitude = FLT_MAX;
			for (uint8_t c = 0; c < num_channels; ++c) {
				const float amplitude = sqrt(real[f][c] * real[f][c] + imag[f][c] * imag[f][c]) / avg_amplitude[c];
				if (amplitude < min_amplitude) min_amplitude = amplitude;
			}

			if (min_amplitude > max_amplitude[f]) {
				max_amplitude[f] = min_amplitude;
				max_time[f] = time;
				for (uint8_t c = 0; c < num_channels; ++c)
					max_phase[f][c] = atan2(imag[f][c], real[f][c]);
			}

			// if highest in time window, report
			if (time - max_time[f] >= window && max_amplitude[f] >= threshold) {
				const float base_phase = max_phase[f][0];
				out_idx += snprintf(out + out_idx, len_out - out_idx, "%i ", f);
				for (uint8_t c = 1; c < num_channels; ++c) {
					float phase_shift = max_phase[f][c] - base_phase;

					while (phase_shift < -M_PI) phase_shift += 2 * M_PI;
					while (phase_shift >= M_PI) phase_shift -= 2 * M_PI;

					const float time_diff = phase_shift / (2 * M_PI * frequencies[f]) * 1000000;

					out_idx += snprintf(out + out_idx, len_out - out_idx, "%.2f ", time_diff);
				}
				out_idx += snprintf(out + out_idx, len_out - out_idx, "%.2f\n", max_amplitude[f]);

				max_amplitude[f] = -1;
				max_time[f] = time;

				for (uint8_t c = 0; c < num_channels; ++c) {
					real[f][c] = 0;
					imag[f][c] = 0;
				}
			}
		}
	}

	++iter;

	return out;
}

