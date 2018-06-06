#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "global.hpp"

static constexpr const float decay = .99;
static constexpr const float window = 1.996;
static constexpr const float threshold = .0;
static constexpr const bool normalize = false;

static const float frequencies[] = {
	25000,
	30000,
	35000,
	40000,
};
static const int8_t  num_frequencies = sizeof(frequencies) / sizeof(float);
static const uint64_t gcd = 5000;
static const uint64_t period = sampling_rate / gcd;

static float avg_amplitude[num_channels];

void init_process() {
	for (uint8_t i = 0; i < num_frequencies; ++i)
		avg_amplitude[i] = 1;
}

static float max_amplitude[num_frequencies] = {}; // the highest recently observed amplitude
static float max_time[num_frequencies] = {}; // the time of the highest observed amplitude
static float max_phase[num_frequencies][num_channels] = {}; // the phase of each channel when the highest amplitude was observed

static float real[num_frequencies][num_channels] = {};
static float imag[num_frequencies][num_channels] = {};

static char out[1024];
static const size_t len_out = sizeof(out) / sizeof(char);
static uint64_t iter = 0;

const char* process(const int16_t* const in) {
	size_t out_idx = 0;
	out[0] = '\0';

	const float time = float(iter) / sampling_rate;

	// dft for only the relavent frequencies
	for (uint8_t f = 0; f < num_frequencies; ++f) {
		for (uint8_t c = 0; c < num_channels; ++c) {
			const float angle = 2 * M_PI * frequencies[f] * time;

			const float v = float(in[c]) / 32768;

			real[f][c] *= decay;
			real[f][c] += v * cos(angle) * (1 - decay);

			imag[f][c] *= decay;
			imag[f][c] -= v * sin(angle) * (1 - decay);
		}
	}

	// compute averages for each channel
	if (normalize) {
		for (uint8_t f = 0; f < num_frequencies; ++f) {
			avg_amplitude[f] = avg_amplitude[f] * decay;
			for (uint8_t c = 0; c < num_channels; ++c) {
				const float amplitude = sqrt(real[f][c] * real[f][c] + imag[f][c] * imag[f][c]);
				avg_amplitude[c] += amplitude / num_channels * (1 - decay);
			}
		}
	}

	// find time for which the minimum amplitude is highest
	if (time > window) {
		for (uint8_t f = 0; f < num_frequencies; ++f) {
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

	if (iter == period * window * sampling_rate) {
		iter = 0;
		for (uint8_t f = 0; f < num_frequencies; ++f) {
			max_time[f] = 0;
			max_amplitude[f] = FLT_MIN;
		}
	}

	return out;
}

