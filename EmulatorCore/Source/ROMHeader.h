#pragma once
#include "Globals.h"

struct ROMHeader {
	uint32_t size = 0;

	bool fourPage = false;
	bool verticalMirroring = false;
	bool hasTrainer = false;
	bool hasPRGRAM = false;
	bool PALmode = false;

	uint32_t CHRROMcount = 0;
	uint32_t PRGROMcount = 0;
	uint32_t PRGRAMcount = 0;
	uint32_t mapper = 0;

	ROMHeader() {}

	ROMHeader(const uint32_t romSize, const uint8_t* data) {
		size = romSize;
		fourPage = data[6] & 0b100;
		verticalMirroring = (data[6] & 1);
		PALmode = data[9] & 0b1;
		hasPRGRAM = data[6] & 0b10;
		hasTrainer = data[6] & 0b100;
		PRGROMcount = data[4];
		CHRROMcount = data[5];
		PRGRAMcount = data[8];
		mapper = (data[6] >> 4) | (data[7] & 0xf0);
	}

	void Print() {
		std::cout << "ROM size: " << size << "\n";
		std::cout << "PRG ROMs: " << PRGROMcount << "\n";
		std::cout << "CHR ROMs: " << CHRROMcount << "\n";
		std::cout << "PRG RAMs: " << PRGRAMcount << "\n";
		std::cout << "Mapper: " << mapper << "\n";
		std::cout << "Scrolling: " << (fourPage ? "4 page" : (verticalMirroring ? "vertical" : "horizontal")) << "\n";
		std::cout << "Has PRGRAM: " << (hasPRGRAM ? "true" : "false") << "\n";
		std::cout << "Has Trainer: " << (hasTrainer ? "true" : "false") << "\n";
		std::cout << "TV system: " << (PALmode ? "PAL" : "NTSC") << "\n";
		std::cout << "\n";
	}
};

