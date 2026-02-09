#include <fstream>
#include <cstdio>
#include <thread>
#include "chip8.h"

const uint8_t keymap[16] = {
	SDLK_0,
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_4,
	SDLK_5,
	SDLK_6,
	SDLK_7,
	SDLK_8,
	SDLK_9,
	SDLK_A,
	SDLK_B,
	SDLK_C,
	SDLK_D,
	SDLK_E,
	SDLK_F
};

const uint8_t fontset[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
{
	draw = false;
	for (int i = 0; i < 16; ++i)
		v[i] = 0x00;

	I	= 0x0000;
	pc	= 0x0200;

	for (int i = 0; i < 16; ++i)
		key[i] = 0x00;

	sp	= 0;

	for (int i = 0; i < 2048; ++i)
		gfx[i] = 0x00;

	for (int i = 0; i < 80; ++i)
		memory[0x50 + i] = fontset[i];

	delay_timer = 0;
	sound_timer = 0;

	srand(time(NULL));
}

void Chip8::run(char* rom_path)
{
	std::ifstream rom(rom_path, std::ios::binary | std::ios::ate);
	std::streamsize size = rom.tellg();

	rom.seekg(0, std::ios::beg);
	rom.read((char*)memory + 0x0200, size);

	while (1) {
		const uint16_t opcode = fetch();
		execute(opcode);

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT)
				exit(0);

			if (e.type == SDL_EVENT_KEY_DOWN) {
				for (int i = 0; i < 16; ++i) {
					if (e.key.key == keymap[i]) {
						key[i] = 1;
					}
				}
			}

			if (e.type == SDL_EVENT_KEY_UP) {
				for (int i = 0; i < 16; ++i) {
					if (e.key.key == keymap[i]) {
						key[i] = 0;
					}
				}
			}
		}

		if (draw) {
			display.render_frame(gfx);
			draw = false;
		}

		std::this_thread::sleep_for(std::chrono::microseconds(200));
	}
}

void Chip8::execute(uint16_t opcode)
{
	switch (opcode & 0xF000) {

	// 0NNN, 00E0, 00EE
	case 0x0000:
		switch (opcode & 0x000F) {
		// 00E0: clear the screen
		case 0x0000:
			for (int i = 0; i < 2048; ++i)
				gfx[i] = 0;
			draw = true;
			break;

		// 00EE: return from subroutine
		case 0x000E:
			--sp;
			pc = stack[sp];
			break;
		}
		break;

	// 1NNN: jump to NNN
	case 0x1000:
		pc = opcode & 0x0FFF;
		break;

	// 2NNN: call subroutine at NNN
	case 0x2000:
		stack[sp] = pc;
		++sp;
		pc = opcode & 0x0FFF;
		break;

	// 3XNN: skip next inst if VX == NN
	case 0x3000:
		if (v[(opcode & 0x0F00) >> 8] ==
			(opcode & 0x00FF))
			pc += 2;
		break;

	// 4XNN: skip next inst if VX != NN
	case 0x4000:
		if (v[(opcode & 0x0F00) >> 8] !=
			(opcode & 0x00FF))
			pc += 2;
		break;

	// 5XY0: skip next inst if VX == VY
	case 0x5000:
		if (v[(opcode & 0x0F00) >> 8] ==
			v[(opcode & 0x00F0) >> 4])
			pc += 2;
		break;

	// 6XNN: set Vx to NN
	case 0x6000:
		v[(opcode & 0x0F00) >> 8] =
			opcode & 0x00FF;
		break;

	// 7XNN: add NN to Vx
	case 0x7000:
		v[(opcode & 0x0F00) >> 8] +=
			opcode & 0x00FF;
		break;

	// 8XY0, 8XY1, ... 8XY7, 8XYE
	case 0x8000:
		switch (opcode & 0x000F) {
		// 8XY0: Vx = Vy
		case 0x0000:
			v[(opcode & 0x0F00) >> 8] =
				v[(opcode & 0x00F0) >> 4];
			break;

		// 8XY1: Vx |= Vy
		case 0x0001:
			v[(opcode & 0x0F00) >> 8] |=
				v[(opcode & 0x00F0) >> 4];
			break;

		// 8XY2: Vx &= Vy
		case 0x0002:
			v[(opcode & 0x0F00) >> 8] &=
				v[(opcode & 0x00F0) >> 4];
			break;

		// 8XY3: Vx ^= Vy
		case 0x0003:
			v[(opcode & 0x0F00) >> 8] ^=
				v[(opcode & 0x00F0) >> 4];
			break;

		// 8XY4: Vx += Vy
		case 0x0004:
		{
			uint8_t x = (opcode & 0x0F00) >> 8;
			uint8_t y = (opcode & 0x00F0) >> 4;
			uint8_t t = (v[x] + v[y]) > 0xFF;
			v[x] += v[y];
			v[15] = t;
			break;
		}

		// 8XY5: Vx -= Vy
		case 0x0005:
		{
			uint8_t x = (opcode & 0x0F00) >> 8;
			uint8_t y = (opcode & 0x00F0) >> 4;
			uint8_t t = v[x] >= v[y];
			v[x] -= v[y];
			v[15] = t;
			break;
		}

		// 8XY6: Vx >>= 1
		case 0x0006:
		{
			uint8_t t = v[(opcode & 0x0F00) >> 8] & 0x01;
			v[(opcode & 0x0F00) >> 8] >>= 1;
			v[15] = t;
			break;
		}

		// 8XY7: Vx = Vy - Vx
		case 0x0007:
		{
			uint8_t x = (opcode & 0x0F00) >> 8;
			uint8_t y = (opcode & 0x00F0) >> 4;
			uint8_t t = v[y] >= v[x];
			v[x]  = v[y] - v[x];
			v[15] = t;
			break;
		}
		
		// 8XYE: Vx <<= 1
		case 0x000E:
		{
			uint8_t t = (v[(opcode & 0x0F00) >> 8] & 0x80) >> 7;
			v[(opcode & 0x0F00) >> 8] <<= 1;
			v[15] = t;
			break;
		}
		}
		break;

	// 9XY0: skip next inst if VX != VY
	case 0x9000:
		if (v[(opcode & 0x0F00) >> 8] !=
			v[(opcode & 0x00F0) >> 4])
			pc += 2;
		break;

	// ANNN: set I to the address NNN
	case 0xA000:
		I = opcode & 0x0FFF;
		break;

	// BNNN: pc = V0 + NNN
	case 0xB000:
		pc = v[0] + (opcode & 0x0FFF);
		break;

	// CXNN: Vx = rand() & NN
	case 0xC000:
		v[(opcode & 0x0F00) >> 8] = (rand() % 0x100) &
			(opcode & 0x00FF);
		break;

	// DXYN: draw(Vx, Vy, N)
	case 0xD000:
	{
		uint8_t x = v[(opcode & 0x0F00) >> 8];
		uint8_t y = v[(opcode & 0x00F0) >> 4];
		uint8_t n = opcode & 0x000F;

		v[15] = 0;
		for (int row = 0; row < n; ++row) {
			uint8_t pixel = memory[I + row];
			for (int col = 0; col < 8; ++col) {
				if ((pixel & (0x80 >> col)) != 0) {
					if (gfx[x + col + (y + row) * 64] == 1)
						v[15] = 1;
					gfx[x + col + ((y + row) * 64)] ^= 1;
				}
			}
		}
		draw = true;
		break;
	}

	// EX9E, EXA1
	case 0xE000:
		switch (opcode & 0x00FF) {
		// EX9E: skip next inst if key() == Vx
		case 0x009E:
			if (key[v[(opcode & 0x0F00) >> 8]] != 0)
				pc += 2;
			break;

		// EXA1: skip next inst if key() != Vy
		case 0x00A1:
			if (key[v[(opcode & 0x0F00) >> 8]] == 0)
				pc += 2;
		}
		break;

	// FX07, FX0A, FX15, FX18, FX1E, FX29, FX33, FX55, FX65
	case 0xF000:
		switch (opcode & 0x00FF) {
		// FX07: set VX to the value of the delay timer
		case 0x0007:
			v[(opcode & 0x0F00) >> 8] = delay_timer;
			break;

		// FX0A: a key press is awaited, and then stored in VX
		case 0x000A:
		{
			bool pressed = false;
			for (int i = 0; i < 16; ++i) {
				if (key[i] != 0) {
					v[(opcode & 0x0F00) >> 8] = i;
					pressed = true;
				}
			}
			if (!pressed)
				pc -= 2;
			break;
		}

		// FX15: set the delay timer to VX
		case 0x0015:
			delay_timer = v[(opcode & 0x0F00) >> 8];
			break;

		// FX18: set the sound timer to VX
		case 0x0018:
			sound_timer = v[(opcode & 0x0F00) >> 8];
			break;

		// FX1E: I += Vx
		case 0x001E:
			I += v[(opcode & 0x0F00) >> 8];
			break;

		// FX29: set I to the location of the sprite for the character in VX
		case 0x0029:
			I = v[(opcode & 0x0F00) >> 8] * 5;
			break;

		// FX33: store BCD representation of VX
		case 0x0033:
			memory[I] = v[(opcode & 0x0F00) >> 8] / 100;
			memory[I + 1] = (v[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I + 2] = v[(opcode & 0x0F00) >> 8] % 10;
			break;

		// FX55: store V0 to VX (including VX) in memory 
		// starting at address I
		case 0x0055:
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
				memory[I + i] = v[i];
			break;

		// FX65: fill from V0 to VX (including VX) with values
		// from memory, starting at address I
		case 0x0065:
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
				v[i] = memory[I + i];
			break;
		}
		break;
	}

	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
		--sound_timer;
}