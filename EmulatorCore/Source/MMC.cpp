#include "MMC.h"

MMC::MMC(Bus* interconnect) {
	bus = interconnect;
	mapper = new NMAP();
}

MMC::~MMC() {
	delete mapper;
}

void MMC::Reset() {
	mapper->Reset();
}

uint8_t MMC::Read(uint16_t address, bool fetch) {
	return mapper->Read(address, fetch);
}

void MMC::Write(uint16_t address, uint8_t value) {
	mapper->Write(address, value);
}

uint8_t MMC::ReadRAM(uint16_t address, bool fetch) {
	return ram[address % ram.size()];
}

void MMC::WriteRAM(uint16_t address, uint8_t value) {
	ram[address % ram.size()] = value;
}

uint8_t MMC::ReadVRAM(uint16_t address) {
	if (address >= 0x2000 && address < 0x3f00) {
		uint8_t nametable = (address % 0x1000) / 0x400;
		switch (vramMirroring) {
		case MirroringMode::PAGE0:
			nametable = 0;
			break;
		case MirroringMode::PAGE1:
			nametable = 1;
			break;
		case MirroringMode::VERTICAL:
			nametable &= 0b1;
			break;
		case MirroringMode::HORIZONTAL:
			nametable &= 0b10;
			break;
		case MirroringMode::FULL:
		default:
			break;
		}
		address = 0x2000 + 0x400 * nametable + (address % 0x400);
	}
	else if (address >= 0x3f00) {
		if (address % 0x20 == 0x10) address -= 0x10;
		return vram[0x3f00 + address % 0x20];
	}
	return vram[address % vram.size()];
}

void MMC::WriteVRAM(uint16_t address, uint8_t value) {
	if (address >= 0x2000 && address < 0x3f00) {
		uint8_t nametable = (address % 0x1000) / 0x400;
		switch (vramMirroring) {
		case MirroringMode::PAGE0:
			nametable = 0;
			break;
		case MirroringMode::PAGE1:
			nametable = 1;
			break;
		case MirroringMode::VERTICAL:
			nametable &= 0b1;
			break;
		case MirroringMode::HORIZONTAL:
			nametable &= 0b10;
			break;
		case MirroringMode::FULL:
		default:
			break;
		}
		address = 0x2000 + 0x400 * nametable + (address % 0x400);
	}
	else if (address >= 0x3f00) {
		if (address % 0x20 == 0x10) address -= 0x10;
		address = 0x3f00 + address % 0x20;
	}
	vram[address % vram.size()] = value;
}

uint8_t MMC::ReadOAM(uint8_t address) {
	return oam[address];
}

void MMC::WriteOAM(uint8_t address, uint8_t value) {
	oam[address] = value;
}

OAMEntry MMC::GetOAMEntry(uint8_t address) {
	return *((OAMEntry*)(&oam[address - address % 4]));
}

bool MMC::LoadROM(const uint8_t* data) {
	if (mapper) {
		delete mapper;
	}

	for (int32_t i = prgBatchSize * bus->romInfo.PRGROMcount, j = 0; j < chrBatchSize; i++, j++) vram[j] = data[i];

	if (bus->romInfo.fourPage) {
		vramMirroring = MirroringMode::FULL;
	}
	else if (bus->romInfo.verticalMirroring) {
		vramMirroring = MirroringMode::VERTICAL;
	}
	else {
		vramMirroring = MirroringMode::HORIZONTAL;
	}

	mapper = CreateMapper(data);

	return mapper != nullptr;
}

Mapper* MMC::CreateMapper(const uint8_t* data) {
	switch (bus->romInfo.mapper) {
	case 0:
	default:
		return new NMAP(bus->romInfo, data);
	}
}
