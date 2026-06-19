#pragma once
#include "Globals.h"
#include "MMC.h"

class MMC1 : public MMC {
	uint8_t shift = 0;
	uint8_t shiftsCount = 0;
	union {
		struct {
			uint8_t mirroring : 2;
			uint8_t prgBankMode : 2;
			uint8_t chrBankMode : 1;
			uint8_t controlUnused : 3;
		};
		uint8_t control = 0;
	};
	uint8_t chrBank0 = 0;
	uint8_t chrBank1 = 0;
	uint8_t prgBank = 0;

	std::vector<std::array<uint8_t, prgBankSize>> prgBanks;
	std::vector<std::array<uint8_t, chrBankSize>> chrBanks;
	std::array<uint8_t, 0x4000> vram = std::array<uint8_t, 0x4000>();
	std::array<uint8_t, 0x2000> ram;
public:
	MMC1(Bus& bus, const ROMHeader& header, const uint8_t* data);

	void Reset();
	uint8_t Read(uint16_t address, bool fetch = false);
	void Write(uint16_t address, uint8_t value);
	uint8_t ReadVRAM(uint16_t address);
	void WriteVRAM(uint16_t address, uint8_t value);
};