#include <stdio.h>

void init_output() {
}

void output(const char* const out) {
	printf("%s", out);
	fflush(stdout);
}

