#pragma once
#include "Globals.h"
#include "Bus.h"
#include "PPU.h"
#include "OAMEntry.h"
#include "Mapper.h"

class MMC {
	Bus* bus = nullptr;
	Mapper* mapper = nullptr;

	std::array<uint8_t, 0x800> ram = std::array<uint8_t, 0x800>();
	std::array<uint8_t, 0x4000> vram = std::array<uint8_t, 0x4000>();
	std::array<uint8_t, 0x100> oam = std::array<uint8_t, 0x100>();
	MirroringMode vramMirroring = MirroringMode::FULL;

public:
	MMC(Bus* interconnect);
	~MMC();

	void Reset();
	bool LoadROM(const uint8_t* data);

	uint8_t Read(uint16_t address, bool fetch = false);
	void Write(uint16_t address, uint8_t value);
	uint8_t ReadRAM(uint16_t address, bool fetch = false);
	void WriteRAM(uint16_t address, uint8_t value);
	uint8_t ReadVRAM(uint16_t address);
	void WriteVRAM(uint16_t address, uint8_t value);
	uint8_t ReadOAM(uint8_t address);
	void WriteOAM(uint8_t address, uint8_t value);
	OAMEntry GetOAMEntry(uint8_t address);

private:
	Mapper* CreateMapper(const uint8_t* data);
};

