#include <stdint.h>

void init_input();
void init_process();
void init_output();

const uint16_t* input();
const char* process(const uint16_t* const in);
void output(const char* const out);

int main() {
	init_input();
	init_process();
	init_output();

	while (true) {
		const uint16_t* const in = input();
		const char* const out = process(in);
		output(out);
	}
}

