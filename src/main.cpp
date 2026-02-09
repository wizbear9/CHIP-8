#include <cstdio>
#include "chip8.h"

int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("usage: %s <ROM_path>\n", argv[0]);
		return 0;
	}

	Chip8 ch8;
	ch8.run(argv[1]);

	return 0;
}