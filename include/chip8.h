#ifndef _CHIP_8_
#define _CHIP_8_

#include <SDL3/SDL.h>
#include <cstdint>
#include "display.h"

class Chip8
{
public:
	Chip8();

	void run(char *rom_path);

private:
	// members
	Display display;
	bool draw;

	uint8_t v[16];			// 8-bit registers: v0-14, flag
	uint16_t I;				// index register
	uint16_t pc;			// program counter
	uint16_t key[16];		// keypad state

	uint8_t memory[0x1000];	// 4096 bytes

	uint16_t stack[16];
	uint16_t sp;

	uint8_t gfx[64 * 32];	// 2048 pixels

	uint8_t delay_timer;
	uint8_t sound_timer;

	// methods
	uint16_t fetch();
	void execute(uint16_t);
};

inline uint16_t Chip8::fetch()
{
	uint16_t inst = memory[pc] << 8 | 
		memory[pc + 1];
	pc += 2;
	return inst;
}

#endif