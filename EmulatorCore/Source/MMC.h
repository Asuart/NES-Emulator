#pragma once
#include "Globals.h"
#include "OAMEntry.h"
#include "Bus.h"

class MMC {
protected:
	Bus* bus = nullptr;

	std::array<uint8_t, 0x800> ram = std::array<uint8_t, 0x800>();
	std::array<uint8_t, 0x100> oam = std::array<uint8_t, 0x100>();
	MirroringMode vramMirroring = MirroringMode::FULL;

public:
	MMC(Bus* bus);

	virtual void Reset() = 0;
	virtual uint8_t Read(uint16_t address, bool fetch = false) = 0;
	virtual void Write(uint16_t address, uint8_t value) = 0;
	virtual uint8_t ReadVRAM(uint16_t address) = 0;
	virtual void WriteVRAM(uint16_t address, uint8_t value) = 0;

	uint8_t ReadRAM(uint16_t address, bool fetch = false);
	void WriteRAM(uint16_t address, uint8_t value);
	uint8_t ReadOAM(uint8_t address);
	void WriteOAM(uint8_t address, uint8_t value);
	OAMEntry GetOAMEntry(uint8_t address);
};

