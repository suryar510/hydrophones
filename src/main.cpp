#include <stdint.h>

#include "global.hpp"

void init_input();
void init_process();
void init_output();

int16_t (*input())[block_size];
const char* process(int16_t (* const in)[block_size]);
void output(const char* const out);

int main() {
	init_input();
	init_process();
	init_output();

	while (true) {
		int16_t (* const in)[block_size] = input();
		const char* const out = process(in);
		output(out);
	}
}

