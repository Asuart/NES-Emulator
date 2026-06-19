#include "MMC.h"

MMC::MMC(Bus& bus) : bus(bus) {
	if (bus.romInfo.fourPage) {
		vramMirroring = MirroringMode::FULL;
	}
	else if (bus.romInfo.verticalMirroring) {
		vramMirroring = MirroringMode::VERTICAL;
	}
	else {
		vramMirroring = MirroringMode::HORIZONTAL;
	}
}

uint8_t MMC::ReadRAM(uint16_t address, bool fetch) {
	return ram[address % ram.size()];
}

void MMC::WriteRAM(uint16_t address, uint8_t value) {
	ram[address % ram.size()] = value;
}

uint8_t MMC::ReadOAM(uint8_t address) {
	return oam[address];
}

void MMC::WriteOAM(uint8_t address, uint8_t value) {
	oam[address] = value;
}

const OAMEntry& MMC::GetOAMEntry(uint8_t address) {
	return *((OAMEntry*)(&oam[address - address % 4]));
}

OAMEntry* MMC::GetOAMBuffer() {
	return (OAMEntry*)oam.data();
}