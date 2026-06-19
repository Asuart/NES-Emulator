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
	Bus bus;
	CPU cpu;
	PPU ppu;
	IO io;
	APU apu;
	MMC* mmc;

	bool m_romLoaded = false;
	int32_t m_cycleAligner = 0;

	NesEmulator();
	~NesEmulator();

	void Reset();
	void Run(int32_t cycles);
	bool LoadROM(const uint8_t* data, const uint32_t size);
private:

	MMC* CreateMapper(const uint8_t* data);
};

