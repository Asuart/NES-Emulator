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

	Bus(NesEmulator* emulator);

	void Reset();

	uint8_t Read(uint16_t address, bool fetch = false);
	void Write(uint16_t address, uint8_t value);
	uint8_t ReadVRAM(uint16_t address);
	void WriteVRAM(uint16_t address, uint8_t value);
	uint8_t ReadOAM(uint8_t address);
	void WriteOAM(uint8_t address, uint8_t value);
	OAMEntry GetOAMEntry(uint8_t index);

	void TriggerNMI();

private:
	NesEmulator* emulator;
	CPU* cpu = nullptr;
	MMC* mmc = nullptr;
	PPU* ppu = nullptr;
	IO* io = nullptr;
	APU* apu = nullptr;

	friend class NesEmulator;
};

