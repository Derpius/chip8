#include "VM.h"

#include "Font.h"
#include "Opcode.h"

#include <cstring>
#include <stdexcept>

Chip8::Chip8()
{
	Reset();
}

void Chip8::Reset()
{
	// Clear memory
	memset(RAM, 0, sizeof(RAM));
	memset(VRAM, 0, sizeof(VRAM));

	// Reset registers
	memset(V, 0, sizeof(V));
	I = 0;

	DT = 0;
	ST = 0;

	PC = 0x200;
	SP = 0;

	// Reset stack
	memset(stack, 0, sizeof(stack));

	// Load font into interpreter memory
	memcpy(RAM, FONT, sizeof(FONT));

	// Reset timer accumulator
	timerAcc = 0.f;
}

void Chip8::LoadProgram(const uint8_t* pProgramData, size_t size)
{
	Reset();

	if (size > sizeof(RAM) - 0x200) size = sizeof(RAM) - 0x200;
	memcpy(RAM + 0x200, pProgramData, size);
}

const uint8_t* Chip8::GetVRAM() const { return VRAM; }

void Chip8::Call(const uint16_t addr)
{
	if (SP == sizeof(stack)) throw std::runtime_error("Stack overflow");
	stack[SP++] = PC;
	PC = addr;
}

void Chip8::Return()
{
	if (SP == 0) throw std::runtime_error("Stack underflow");
	PC = stack[--SP];
}

inline uint16_t be16_to_cpu(const uint8_t* buf)
{
	return ((uint16_t)buf[1]) | (((uint16_t)buf[0]) << 8);
}

void Chip8::Tick(const float deltaTime)
{
	if (ST > 0 || DT > 0) {
		timerAcc += deltaTime;
		if (timerAcc > 1.f / 60.f) {
			if (ST > 0) ST--;
			if (DT > 0) DT--;

			timerAcc = 0.f;
		}
	}

	Opcode op{ be16_to_cpu(RAM + PC) };
	PC += 2;

	switch (op.Type()) {
	case 0x0:
		if (op.raw == 0x00E0) {
			memset(VRAM, 0, sizeof(VRAM));
			break;
		}
		if (op.raw == 0x00EE) {
			Return();
			break;
		}
		break;
	case 0x1:
		PC = op.N<3>();
		break;
	case 0x2:
		Call(op.N<3>());
		break;
	case 0x3:
		if (V[op.X()] == op.N<2>()) PC += 2;
		break;
	case 0x4:
		if (V[op.X()] != op.N<2>()) PC += 2;
		break;
	case 0x5:
		if (V[op.X()] == V[op.Y()]) PC += 2;
		break;
	case 0x6:
		V[op.X()] = op.N<2>();
		break;
	case 0x7:
		V[op.X()] += op.N<2>();
		break;
	case 0x8:
		switch (op.N<1>()) {
		case 0x0:
			V[op.X()] = V[op.Y()];
			break;
		case 0x1:
			V[op.X()] |= V[op.Y()];
			break;
		case 0x2:
			V[op.X()] &= V[op.Y()];
			break;
		case 0x3:
			V[op.X()] ^= V[op.Y()];
			break;
		case 0x4:
		{
			int overflowable = static_cast<int>(V[op.X()]) + V[op.Y()];
			if (overflowable > 255) {
				V[op.X()] = overflowable & 0xFF;
				V[0xF] = 1;
			} else {
				V[op.X()] = overflowable;
				V[0xF] = 0;
			}
			break;
		}
		case 0x5:
			V[0xF] = V[op.X()] > V[op.Y()] ? 1 : 0;
			V[op.X()] -= V[op.Y()];
			break;
		case 0x6:
			V[0xF] = (V[op.X()] & 1) ? 1 : 0;
			V[op.X()] >>= 1;
			break;
		case 0x7:
			V[0xF] = V[op.Y()] > V[op.X()] ? 1 : 0;
			V[op.X()] = V[op.Y()] - V[op.X()];
			break;
		case 0xE:
			V[0xF] = (V[op.X()] & 0b10000000) ? 1 : 0;
			V[op.X()] <<= 1;
			break;
		}
		break;
	case 0x9:
		if (V[op.X()] != V[op.Y()]) PC += 2;
		break;
	case 0xA:
		I = op.N<3>();
		break;
	case 0xB:
		PC = op.N<3>() + V[0];
		break;
	case 0xC:
		break;
	case 0xD:
	{
		uint8_t xOrig = V[op.X()] % 64, yOrig = V[op.Y()] % 32;
		V[0xF] = 0;

		for (uint8_t row = 0; row < op.N<1>(); row++) {
			uint8_t sprite = RAM[I + row];
			uint8_t y = yOrig + row;

			for (int bit = 7; bit >= 0; bit--) {
				uint8_t x = xOrig + 7 - bit;

				if ((sprite >> bit) & 1) {
					uint8_t* pixel = VRAM + y * 64 + x;
					if (*pixel) {
						*pixel = 0;
						V[0xF] = 1;
					} else {
						*pixel = 0xFF;
					}
				}

				if (x >= 63) break;
			}

			if (y >= 31) break;
		}

		break;
	}
	case 0xE:
		break;
	case 0xF:
		break;
	}
}
