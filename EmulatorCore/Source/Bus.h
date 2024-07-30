#pragma once
#include "Globals.h"
#include "ROMHeader.h"
#include "OAMEntry.h"
#include "CPU.h"
#include "PPU.h"
#include "IO.h"
#include "APU.h"
#include "MMC.h"

class Bus {
public:
	ROMHeader romInfo;

	Bus(NesEmulator& emulator, CPU& cpu, PPU& ppu, IO& io, APU& apu, MMC* mmc);

	void Reset();

	uint8_t Read(uint16_t address, bool fetch = false);
	void Write(uint16_t address, uint8_t value);
	uint8_t ReadVRAM(uint16_t address);
	void WriteVRAM(uint16_t address, uint8_t value);
	uint8_t ReadOAM(uint8_t address);
	void WriteOAM(uint8_t address, uint8_t value);
	const OAMEntry& GetOAMEntry(uint8_t index);
	OAMEntry* GetOAMBuffer();

	void TriggerNMI();

private:
	NesEmulator& emulator;
	CPU& cpu;
	PPU& ppu;
	IO& io;
	APU& apu;
	MMC* mmc;

	friend class NesEmulator;
};

