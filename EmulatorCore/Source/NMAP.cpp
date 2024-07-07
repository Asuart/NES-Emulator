#include "NMAP.h"

NMAP::NMAP(Bus* bus) : MMC(bus) {}

NMAP::NMAP(Bus* bus, const ROMHeader& header, const uint8_t* data)
	: MMC(bus) {
	if (header.PRGROMcount == 1) {
		for (int32_t i = 0; i < prgBankSize; i++)
			prg0[i] = prg1[i] = data[i];
	} else if (header.PRGROMcount == 2) {
		for (int32_t i = 0; i < prgBankSize; i++)
			prg1[i] = data[i];
		for (int32_t i = prgBankSize; i < prgBankSize * 2; i++)
			prg0[i - prgBankSize] = data[i];
	} else {
		std::cout << "Error: NMAP only support up to 2 prg banks.\n";
	}

	if (bus->romInfo.CHRROMcount < 2) {
		for (int32_t i = prgBankSize * bus->romInfo.PRGROMcount, j = 0; j < chrBankSize; i++, j++) vram[j] = data[i];
	}
	else {
		std::cout << "Error: NMAP only support up to 1 chr bank.\n";
	}
}

void NMAP::Reset() {}

uint8_t NMAP::Read(uint16_t address, bool fetch) {
	if (address >= 0xc000) return prg0[address - 0xc000];
	else if (address >= 0xffff) return prg1[address - 0x8000];
	else {
		std::cout << "Warning: Read from unhandled cartridge space: 0x" << std::hex << address << "\n";
	}
	return 0xff;
}

void NMAP::Write(uint16_t address, uint8_t value) {
	std::cout << "Warning: Write to unhandled cartridge space: 0x" << std::hex << address << "\n";
}

uint8_t NMAP::ReadVRAM(uint16_t address) {
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

void NMAP::WriteVRAM(uint16_t address, uint8_t value) {
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