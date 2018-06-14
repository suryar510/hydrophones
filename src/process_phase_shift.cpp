#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "global.hpp"
#include "fxpt_atan2.hpp"

// times are in uint64_t (micros)
// voltages are in uint16_t (0 - 2^16)
// fourier transform is in int32_t (-2^16 - 2^16)
// squared amplitude is in uint64_t (0 - 2^64)
// phase is in int32_t
// time diff is in int64_t (nanos)

static constexpr const int32_t B16 = 1 << 16;

static constexpr const int32_t alpha = B16 * .99;
static constexpr const uint64_t window = 1996000; // in us
static constexpr const uint32_t threshold = .0;

static const uint64_t frequencies[] = {
	//25000,
	30000,
	//35000,
	//40000,
};
static const int8_t  num_frequencies = sizeof(frequencies) / sizeof(uint64_t);
static const uint64_t gcd = 5000;
static const uint64_t period = sampling_rate / gcd;

static constexpr const uint64_t sine_wave_size = 200;
static int32_t sin_buf[sine_wave_size];
static int32_t cos_buf[sine_wave_size];

void init_process() {
	for (uint64_t i = 0; i < sine_wave_size; ++i) {
		const float angle = 2 * M_PI * i / sine_wave_size;
		sin_buf[i] = sin(angle) * 256 * 256;
		cos_buf[i] = cos(angle) * 256 * 256;
	}
}

static uint64_t max_amplitude[num_frequencies] = {}; // the highest recently observed amplitude
static uint64_t max_time[num_frequencies] = {}; // the time of the highest observed amplitude
static int32_t max_real[num_frequencies][num_channels] = {};
static int32_t max_imag[num_frequencies][num_channels] = {};

static int32_t real[num_frequencies][num_channels] = {};
static int32_t imag[num_frequencies][num_channels] = {};

static char out[1024];
static const size_t len_out = sizeof(out) / sizeof(char);
static uint64_t iter = 0;

static constexpr const int32_t beta = B16 - alpha;

const char* process(const uint16_t* const in) {
	size_t out_idx = 0;
	out[0] = '\0';

	const uint64_t time = iter * 1000000 / sampling_rate; // in uc

	// dft for only the relevant frequencies
	for (uint8_t f = 0; f < num_frequencies; ++f) {
		const uint64_t microrevs = frequencies[f] * time % 1000000; // angle = 2pi * microrevs / 1000000
		const uint64_t wave_idx = microrevs * sine_wave_size / 1000000;
		for (uint8_t c = 0; c < num_channels; ++c) {
			real[f][c] = (real[f][c] * alpha + in[c] * sin_buf[wave_idx] / B16 * beta) / B16;
			imag[f][c] = (imag[f][c] * alpha - in[c] * cos_buf[wave_idx] / B16 * beta) / B16;
		}
	}

	// find time for which the minimum amplitude is highest
	if (time > window) {
		for (uint8_t f = 0; f < num_frequencies; ++f) {
			// handle overflow
			// should happen rarely
			if (time < max_time[f]) max_time[f] = 0;

			uint64_t min_amplitude = -1;
			for (uint8_t c = 0; c < num_channels; ++c) {
				const uint64_t amplitude = uint64_t(int64_t(real[f][c]) * int64_t(real[f][c])) + uint64_t(int64_t(imag[f][c]) * int64_t(imag[f][c]));
				if (amplitude < min_amplitude) min_amplitude = amplitude;
			}

			if (min_amplitude > max_amplitude[f]) {
				max_amplitude[f] = min_amplitude;
				max_time[f] = time;
				for (uint8_t c = 0; c < num_channels; ++c) {
					max_real[f][c] = real[f][c];
					max_imag[f][c] = imag[f][c];
				}
			}

			// if highest in time window, report
			if (time - max_time[f] >= window && max_amplitude[f] >= threshold) {
				const uint32_t base_phase = fxpt_atan2(max_imag[f][0], max_real[f][0]);
				out_idx += snprintf(out + out_idx, len_out - out_idx, "%" PRIu64 "\t", frequencies[f]);
				for (uint8_t c = 1; c < num_channels; ++c) {
					uint32_t phase = fxpt_atan2(max_imag[f][c], max_real[f][c]);
					int32_t phase_shift = int32_t(phase) - base_phase;

					while (phase_shift < -B16 / 2) phase_shift += B16;
					while (phase_shift >= B16 / 2) phase_shift -= B16;

					const int64_t time_diff = int64_t(phase_shift) * 1000000000 / B16 / int64_t(frequencies[f]); // in nanos

					out_idx += snprintf(out + out_idx, len_out - out_idx, "%" PRId64 "\t", time_diff);
				}
				out_idx += snprintf(out + out_idx, len_out - out_idx, "%" PRIu64 "\n", max_amplitude[f]);

				max_amplitude[f] = 0;
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

