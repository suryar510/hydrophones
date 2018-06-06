#include <stdint.h>

void init_input();
void init_process();
void init_output();

const float* input();
const char* process(const float* const in);
void output(const char* const out);

int main() {
	init_input();
	init_process();
	init_output();

	while (true) {
		const float* const in = input();
		const char* const out = process(in);
		output(out);
	}
}

