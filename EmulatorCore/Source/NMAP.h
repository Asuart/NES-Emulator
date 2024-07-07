#pragma once
#include "Globals.h"
#include "MMC.h"

class NMAP : public MMC {
	std::array<uint8_t, 0x4000> prg0 = std::array<uint8_t, 0x4000>();
	std::array<uint8_t, 0x4000> prg1 = std::array<uint8_t, 0x4000>();
	std::array<uint8_t, 0x4000> vram = std::array<uint8_t, 0x4000>();

public:
	NMAP(Bus* bus);
	NMAP(Bus* bus, const ROMHeader& header, const uint8_t* data);

	void Reset();
	uint8_t Read(uint16_t address, bool fetch = false);
	void Write(uint16_t address, uint8_t value);
	uint8_t ReadVRAM(uint16_t address);
	void WriteVRAM(uint16_t address, uint8_t value);
};