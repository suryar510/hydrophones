#include <Arduino.h>
#include <ADC.h>

#include <stdint.h>

#include "global.hpp"

static ADC* adc = new ADC();
static IntervalTimer timer;

// pin[0] = A0 so it can be used by ADC_0
// pin[1] = A14 so it can be used by ADC_1
// A2 and A3 can be used by either ADC
const int base_pins[] = {A0, A14, A2, A3};
int pin[num_channels];

void timer_callback();

void init_input() {
	for (size_t i = 0; i < sizeof(base_pins) / sizeof(int) && i < num_channels; ++i)
		pin[i] = base_pins[i];

	for (size_t i = 4; i < num_channels; ++i)
		pin[i] = A0 + i;

	for (size_t i = 0; i < num_channels; ++i)
		pinMode(pin[i], INPUT);

	delay(100);

	if (num_channels >= 1) {
		adc->setAveraging(1);
		adc->setResolution(12);
		adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
		adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
		adc->enableInterrupts(ADC_0);
	}

	if (num_channels >= 2) {
		adc->setAveraging(1, ADC_1);
		adc->setResolution(12, ADC_1);
		adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_1);
		adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1);
		adc->enableInterrupts(ADC_1);
	}

	static_assert(num_channels >= 2, "need at least 2 channels");
    adc->startSynchronizedContinuous(pin[0], pin[1]);
    timer.begin(timer_callback, int32_t(1000000 / sampling_rate));

	delay(100);
}

static int16_t buffer[2][num_channels][block_size] __attribute__ ((aligned (128)));

static volatile size_t which_buffer = 0;
static volatile size_t buffer_idx = 0;

static volatile bool has_data = false;

int16_t (*input())[block_size] {
	while (!has_data);
	has_data = false;
	return buffer[1 - which_buffer];
}

void timer_callback() {
	const ADC::Sync_result result = adc->readSynchronizedContinuous();
	buffer[which_buffer][0][buffer_idx] = result.result_adc0;
	buffer[which_buffer][1][buffer_idx] = result.result_adc1;
	for (size_t i = 2; i < num_channels; ++i)
		buffer[which_buffer][i][buffer_idx] = result.result_adc1;

	++buffer_idx;
	if (buffer_idx >= block_size) {
		has_data = true;
		buffer_idx = 0;
		which_buffer = 1 - which_buffer;
	}
}

void adc0_isr() {
}

void adc1_isr() {
}

