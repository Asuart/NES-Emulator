#include "MMC1.h"

void MMC1::Reset() {
	shift = 0x00;
	control = 0;
	chrBank0 = 0;
	chrBank1 = 1;
	prgBank = 0;
	prgBankMode = 3;
}

MMC1::MMC1(Bus& bus, const ROMHeader& header, const uint8_t* data)
	: MMC(bus) {
	for (uint32_t i = 0; i < header.PRGROMcount; i++) {
		prgBanks.push_back(std::array<uint8_t, prgBankSize>());
		for (uint32_t j = 0; j < prgBankSize; j++) {
			prgBanks[i][j] = data[i * prgBankSize + j];
		}
	}
	for (uint32_t i = 0; i < header.CHRROMcount; i++) {
		chrBanks.push_back(std::array<uint8_t, chrBankSize>());
		for (uint32_t j = 0; j < chrBankSize; j++) {
			chrBanks[i][j] = data[header.PRGROMcount * prgBankSize + i * chrBankSize + j];
		}
	}
	if (chrBanks.size() < 2) {
		for (size_t i = chrBanks.size(); i < 2; i++) {
			chrBanks.push_back(std::array<uint8_t, chrBankSize>());
			for (uint32_t j = 0; j < chrBankSize; j++) {
				chrBanks[i][j] = 0;
			}
		}
	}
	Reset();
}

uint8_t MMC1::Read(uint16_t address, bool fetch) {
	if (address >= 0x6000 && address < 0x8000) {
		return ram[address - 0x6000];
	}
	switch (prgBankMode) {
	case 0:
	case 1:
		if (address >= 0xc000) {
			return prgBanks[(prgBank & 0b11110) + 1][address - 0xc000];
		}
		else if (address >= 0x8000) {
			return prgBanks[(prgBank & 0b11110)][address - 0x8000];
		}
		else {
			std::cout << "Read from inaccessable address: " << address << "\n";
		}
		break;
	case 2:
		if (address >= 0xc000) {
			return prgBanks[prgBank][address - 0xc000];
		}
		else if (address >= 0x8000) {
			return prgBanks[0][address - 0x8000];
		}
		else {
			std::cout << "Read from inaccessable address: " << address << "\n";
		}
		break;
	case 3:
		if (address >= 0xc000) {
			return prgBanks[prgBanks.size() - 1][address - 0xc000];
		}
		else if (address >= 0x8000) {
			return prgBanks[prgBank][address - 0x8000];
		}
		else {
			std::cout << "Read from inaccessable address: " << address << "\n";
		}
		break;
	}
	return 0;
}

void MMC1::Write(uint16_t address, uint8_t value) {
	if (address & 0x8000) {
		if (value & 0x80) {
			shift = 0x00;
			shiftsCount = 0;
			control = 0x0c;
			vramMirroring = (MirroringMode)mirroring;
		}
		else {
			shift = ((shift >> 1) | ((value & 1) << 4)) & 0b11111;
			shiftsCount++;
			if (shiftsCount == 5) {
				switch ((address >> 13) & 0b11) {
				case 0:
					control = shift;
					vramMirroring = (MirroringMode)mirroring;
					//std::cout << "Set control: " << (uint32_t)control << "\n";
					//std::cout << "Mirroring: " << (uint32_t)mirroring << "\n";
					break;
				case 1:
					chrBank0 = shift;
					break;
				case 2:
					chrBank1 = shift;
					break;
				case 3:
					prgBank = shift;
					break;
				}
				shift = 0x00;
				shiftsCount = 0;
			}
		}
	}
	if (address >= 0x6000 && address < 0x8000) {
		ram[address - 0x6000] = value;
	}
}

uint8_t MMC1::ReadVRAM(uint16_t address) {
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
		return vram[address];
	}
	else if (address >= 0x3f00) {
		if (address % 0x20 == 0x10) address -= 0x10;
		return vram[0x3f00 + address % 0x20];
	}
	else if (chrBankMode) {
		if (address < 0x1000) {
			return chrBanks[chrBank0 % chrBanks.size()][address];
		}
		else {
			return chrBanks[chrBank1 % chrBanks.size()][address - 0x1000];
		}
	}
	else {
		if (address < 0x1000) {
			return chrBanks[(chrBank0 & ~1) % chrBanks.size()][address];
		}
		else {
			return chrBanks[((chrBank0 & ~1) + 1) % chrBanks.size()][address - 0x1000];
		}
	}
}

void MMC1::WriteVRAM(uint16_t address, uint8_t value) {
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
		vram[address] = value;
	}
	else if (address >= 0x3f00) {
		if (address % 0x20 == 0x10) address -= 0x10;
		address = 0x3f00 + address % 0x20;
		vram[address] = value;
	}
	else if (chrBankMode) {
		if (address < 0x1000) {
			chrBanks[chrBank0 % chrBanks.size()][address] = value;
		}
		else {
			chrBanks[chrBank1 % chrBanks.size()][address - 0x1000] = value;
		}
	}
	else {
		if (address < 0x1000) {
			chrBanks[(chrBank0 & ~1) % chrBanks.size()][address] = value;
		}
		else {
			chrBanks[((chrBank0 & ~1) + 1) % chrBanks.size()][address - 0x1000] = value;
		}
	}
}