#include <Arduino.h>
#include <ADC.h>

#include <stdint.h>

#include "global.hpp"

static ADC* adc = new ADC();

static constexpr const ADC_SAMPLING_SPEED SPEED =
	sampling_rate == 94000 ? ADC_SAMPLING_SPEED::LOW_SPEED :
	sampling_rate == 231000 ? ADC_SAMPLING_SPEED::MED_SPEED :
	ADC_SAMPLING_SPEED::HIGH_SPEED
;

// pin[0] = A0 so it can be used by ADC_0
// pin[1] = A14 so it can be used by ADC_1
// A2 and A3 can be used by either ADC
const int base_pins[] = {A0, A14, A2, A3};
int pin[num_channels];

void init_input() {
	for (size_t i = 0; i < sizeof(base_pins) / sizeof(int) && i < num_channels; ++i)
		pin[i] = base_pins[i];

	for (size_t i = 4; i < num_channels; ++i)
		pin[i] = A0 + i;

	for (size_t i = 0; i < num_channels; ++i)
		pinMode(pin[i], INPUT);

	if (num_channels >= 1) {
		adc->setAveraging(1);
		adc->setResolution(12);
		adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
		adc->setSamplingSpeed(SPEED);
		adc->startContinuous(pin[0], ADC_0);
	}

	if (num_channels >= 2) {
		adc->setAveraging(1, ADC_1);
		adc->setResolution(12, ADC_1);
		adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_1);
		adc->setSamplingSpeed(SPEED, ADC_1);
		adc->startContinuous(pin[1], ADC_1);
	}

	delay(500);
}

static int16_t buffer[2][num_channels][block_size];

static volatile size_t which_buffer = 0;
static volatile size_t buffer_idx = 0;

static volatile bool has_data = false;

int16_t (*input())[block_size] {
	while (!has_data);
	has_data = false;
	return buffer[1 - which_buffer];
}

void adc0_isr() {
	if (num_channels >= 1) buffer[which_buffer][0][buffer_idx] = adc->analogReadContinuous(ADC_0);
	if (num_channels >= 2) buffer[which_buffer][1][buffer_idx] = adc->analogReadContinuous(ADC_1);
	if (num_channels == 3) {
		buffer[which_buffer][2][buffer_idx] = adc->analogRead(pin[2]);
	} else if (num_channels == 4) {
		adc->startSingleRead(pin[2], ADC_0);
		buffer[which_buffer][3][buffer_idx] = adc->analogRead(pin[3], ADC_1);
		buffer[which_buffer][2][buffer_idx] = adc->readSingle(ADC_0);
	}
	for (size_t i = 4; i < num_channels; ++i)
		buffer[which_buffer][i][buffer_idx] = adc->analogRead(pin[i]);

	++buffer_idx;

	if (buffer_idx >= block_size) {
		has_data = true;

		buffer_idx = 0;
		which_buffer = 1 - which_buffer;
	}
}

void adc1_isr() {
}

