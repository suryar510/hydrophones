#include <Arduino.h>
#include <ADC.h>

#include <stdint.h>

#include "global.hpp"

static ADC* adc = new ADC();
static IntervalTimer timer;

// A14 and A15 are read by ADC0
// A16 and A17 are read by ADC1
const int base_pins[] = {A14, A15, A16, A17};

void timer_callback();

void init_input() {
	for (size_t i = 0; i < num_channels; ++i)
		pinMode(base_pins[i], INPUT);

	if (num_channels >= 1) {
		adc->setAveraging(1);
		adc->setResolution(12);
		adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
		adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
	}

	if (num_channels >= 2) {
	    	adc->setAveraging(1, ADC_1);
		adc->setResolution(12, ADC_1);
		adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_1);
		adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1);
	}

	adc->printError();
	timer.begin(timer_callback, int32_t(1000000 / sampling_rate));
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
ADC::Sync_result result;
void timer_callback() {
	result = adc->analogSynchronizedRead(pin[0], pin[2]);
	buffer[which_buffer][0][buffer_idx] = result.result_adc0;
	buffer[which_buffer][2][buffer_idx] = result.result_adc1;

	result = adc->analogSynchronizedRead(pin[1], pin[3]);
	buffer[which_buffer][1][buffer_idx] = result.result_adc0;
	buffer[which_buffer][3][buffer_idx] = result.result_adc1;

	++buffer_idx;
	if (buffer_idx >= block_size) {
		has_data = true;
		buffer_idx = 0;
		which_buffer = 1 - which_buffer;
	}
}


