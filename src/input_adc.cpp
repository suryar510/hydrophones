#include <Arduino.h>
#include <ADC.h>

#include <stdint.h>

#include "global.hpp"

ADC* adc = new ADC();

static uint64_t prev_tick = 0;

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
		adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);
		adc->startContinuous(pin[0], ADC_0);
	}

	if (num_channels >= 2) {
		adc->setAveraging(1, ADC_1);
		adc->setResolution(12, ADC_1);
		adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_1);
		adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED, ADC_1);
		adc->startContinuous(pin[1], ADC_1);
	}

	delay(500);
}

static uint16_t in[num_channels];

const uint16_t* input() {
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

	if (num_channels >= 1) in[0] = adc->analogReadContinuous(ADC_0);
	if (num_channels >= 2) in[1] = adc->analogReadContinuous(ADC_1);
	if (num_channels == 3) {
		in[2] = adc->analogRead(pin[2]);
	} else if (num_channels == 4) {
		adc->startSingleRead(pin[2], ADC_0);
		in[3] = adc->analogRead(pin[3], ADC_1);
		in[2] = adc->readSingle(ADC_0);
	}
	for (size_t i = 4; i < num_channels; ++i)
		in[i] = adc->analogRead(pin[i]);

	return in;
}

void adc0_isr(void) {
}

void adc1_isr(void) {
}

