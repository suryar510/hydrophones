#include <stdint.h>

void init_input();
void init_process();
void init_output();

const int16_t* input();
const char* process(const int16_t* const in);
void output(const char* const out);

int main() {
	init_input();
	init_process();
	init_output();

	while (true) {
		const int16_t* const in = input();
		const char* const out = process(in);
		output(out);
	}
}

