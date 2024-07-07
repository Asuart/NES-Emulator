#pragma once
#include "Globals.h"
#include "NMAP.h"
#include "MMC1.h"
#include "Bus.h"
#include "CPU.h"
#include "PPU.h"
#include "IO.h"
#include "APU.h"

class NesEmulator {
public:
	Bus* bus = nullptr;
	CPU* cpu = nullptr;
	MMC* mmc = nullptr;
	PPU* ppu = nullptr;
	IO* io = nullptr;
	APU* apu = nullptr;

	bool romLoaded = false;

	NesEmulator();
	~NesEmulator();

	void Reset();
	void Run(int32_t instructionsCount);
	bool LoadROM(const uint8_t* data, const uint32_t size);

private:
	MMC* CreateMapper(const uint8_t* data);
};

