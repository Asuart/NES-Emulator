#include "Mapper.h"

void Mapper::Reset() {}

NMAP::NMAP(const ROMHeader& header, const uint8_t* data) {
	uint8_t* prg0 = &rom[0xc000 - 0x4020];
	uint8_t* prg1 = &rom[0x8000 - 0x4020];
	if (header.PRGROMcount == 1) for (int32_t i = 0; i < prgBatchSize; i++) prg0[i] = prg1[i] = data[i];
	else for (int32_t i = 0; i < prgBatchSize * 2; i++) prg1[i] = data[i];
}

uint8_t NMAP::Read(uint16_t address, bool fetch) const {
	return rom[address - 0x4020];
}

void NMAP::Write(uint16_t address, uint8_t value) {
	rom[address - 0x4020] = value;
}