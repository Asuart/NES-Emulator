#pragma once
#include "Globals.h"
#include "Bus.h"

class IO {
public:
	Bus* bus = nullptr;
	std::array<uint8_t, 0x20> registers = std::array<uint8_t, 0x20>();
	std::array<uint8_t, 8> keyStates = std::array<uint8_t, 8>();
	uint8_t readKeyNum = 0;

	IO(Bus* bus);

	void Reset();
	uint8_t ReadRegister(uint8_t addr, bool fetch);
	void WriteRegister(uint8_t addr, uint8_t value);
	uint8_t GetKeyState(uint8_t key);
};

