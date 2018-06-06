#include <Arduino.h>

#include <string.h>

#include "global.hpp"

void init_output() {
	Serial.begin(115200); // baud rate ignored
}

void output(const char* const out) {
	Serial.write(out, strlen(out));
}

