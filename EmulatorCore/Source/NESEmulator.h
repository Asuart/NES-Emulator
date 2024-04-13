#pragma once
#include "Globals.h"
#include "Bus.h"
#include "MMC.h"
#include "CPU.h"
#include "PPU.h"
#include "IO.h"

class CPU;
class MMC;
class PPU;
class IO;

class NesEmulator {
public:
	Bus* bus = nullptr;
	CPU* cpu = nullptr;
	MMC* mmc = nullptr;
	PPU* ppu = nullptr;
	IO* io = nullptr;
	bool romLoaded = false;

	NesEmulator();
	~NesEmulator();

	void Reset();
	void Run(int32_t instructionsCount);
	bool LoadROM(const uint8_t* data, const uint32_t size);

protected:
	float ppuClockAligner = 0;
};

