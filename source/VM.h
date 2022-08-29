#pragma once

#include <cstdint>

class Chip8
{
private:
	// Memory
	uint8_t RAM[4096];

	// Display memory
	uint8_t VRAM[64 * 32];

	// General purpose registers
	uint8_t V[16];

	// Memory address register
	uint16_t I;

	// Program counter
	uint16_t PC;

	// Stack pointer
	uint8_t SP;
	uint16_t stack[16];

	// Delay timer register
	uint8_t DT;

	// Sound timer register
	uint8_t ST;

	// Accumulates delta time, and decrements DT and ST when above 1/60th of a second
	float timerAcc;

	void Call(const uint16_t addr);
	void Return();

public:
	Chip8();

	void Tick(const float deltaTime);
	void Reset();

	void LoadProgram(const uint8_t* pProgramData, size_t size);

	const uint8_t* GetVRAM() const;
};
