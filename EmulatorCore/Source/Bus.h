#pragma once
#include "Globals.h"
#include "ROMHeader.h"
#include "NESEmulator.h"
#include "OAMEntry.h"

class NesEmulator;
class CPU;
class MMC;
class PPU;
class IO;

class Bus {
public:
	ROMHeader romInfo;

	Bus(NesEmulator* emulator);

	void Reset();
	bool LoadROM(const uint8_t* data, uint32_t size);

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

	friend class NesEmulator;
};

