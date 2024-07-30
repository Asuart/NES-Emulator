#include "PPU.h"

PPU::PPU(Bus& bus) : bus(bus) {}

void PPU::Reset() {
	ctrl = mask = status = scroll = address = data = dataReadBuffer = 0;
	oamAddress = 0;
	oamData = 0;
	currentScanline = 261;
	currentPixel = 0;
	vramAddress = tempVramAddress = 0x2000;
	currentTile = nextTile = 0;
	currentTileParams = nextTileParams = 0;
	v = t = 0;
	v = t = 0;
	x = 0;
	w = 0;
	frameReady = false;
	clock = 0;
	clockAligner = 0;
}

uint8_t PPU::ReadRegister(uint8_t index, bool fetch) {
	uint8_t returnValue = 0;
	switch (index) {
	case 0:
		return ctrl;
	case 1:
		return mask;
	case 2:
		returnValue = status;
		if (!fetch) {
			verticalBlank = 0;
			w = 0;
		}
		return returnValue;
	case 3:
		return oamAddress;
	case 4:
		returnValue = oamData;
		if (!fetch) {
			oamData = bus.ReadOAM(oamAddress);
			if (!verticalBlank) {
				oamAddress++;
			}
		}
		return returnValue;
	case 5:
		return scroll;
	case 6:
		return address;
	case 7:
		returnValue = dataReadBuffer;
		if (!fetch) {
			dataReadBuffer = bus.ReadVRAM(vramAddress);
			if (vramAddress >= 0x3f00) {
				returnValue = dataReadBuffer;
			}
			if (increment) vramAddress += 32;
			else vramAddress++;
		}
		return returnValue;
	}
	return 0;
}

void PPU::WriteRegister(uint8_t index, uint8_t value) {
	switch (index) {
	case 0:
		ctrl = value;
		t = (t & ~(0b11 << 10)) | ((value & 0b11) << 10);
		if (nmiEnabled) {
			NMI();
		}
		else {
			nmiTriggered = false;
		}
		break;
	case 1:
		mask = value;
		break;
	case 2:
		std::cout << "WARNING::Write to PPUSTATUS: " << (int32_t)value << "\n";
		break;
	case 3:
		oamAddress = value;
		break;
	case 4:
		oamData = value;
		bus.WriteOAM(oamAddress, oamData);
		oamAddress++;
		break;
	case 5:
		scroll = value;

		if (w == 0) {
			t = (t & ~0b11111) | (value >> 3);
			x = value & 0b111;
		}
		else {
			t = (t & ~(0b111 << 12)) | ((value & 0b111) << 12);
			t = (t & ~(0b11111 << 5)) | ((value & 0b11111000) << 2);
		}
		w = !w;

		break;
	case 6:
		address = value;

		if (w == 0) {
			tempVramAddress = (tempVramAddress & 0x00ff) | ((uint16_t)address << 8);

			t = (t & ~(0b111111 << 8)) | ((value & 0b111111) << 8);
			t &= ~(1 << 15);
		}
		else {
			tempVramAddress = (tempVramAddress & 0xff00) | address;
			vramAddress = tempVramAddress;

			t = ((t & ~(0b11111111)) | value);
			v = t;
		}
		w = !w;
		break;
	case 7:
		data = value;
		bus.WriteVRAM(vramAddress, data);
		if (increment) vramAddress += 32;
		else vramAddress++;
		break;
	}
}

void PPU::PresentFrame() {
	frameReady = true;
}

void PPU::Step(uint32_t cpuClocks) {
	clockAligner += cpuClocks * cpuClockRatio;
	while (clockAligner >= 1) {
		clockAligner -= 1.0;
		clock++;

		if (currentScanline < 240) {
			if ((currentPixel != 0) && ((currentPixel % 8) == 0) && ((currentPixel <= 256) || (currentPixel >= 328))) {
				incX();
				if (currentPixel == 256) {
					incY();
				}
			}
			else if (currentPixel == 257) {
				v = (v & ~(0b000010000011111)) | (t & 0b000010000011111);
			}

			if (currentPixel < 256) {
				DrawPixel();
			}
			else if (currentPixel == 257) {
				oamAddress = 0;
				EvaluateSprites();
			}
		}
		else if (currentScanline == 241 && currentPixel == 1) {
			StartVB();
		}
		else if (currentScanline == 261) {
			if ((currentPixel != 0) && ((currentPixel % 8) == 0) && ((currentPixel <= 256) || (currentPixel >= 328))) {
				incX();
				if (currentPixel == 256) {
					incY();
				}
			}
			else if (currentPixel == 257) {
				v = (v & ~(0b000010000011111)) | (t & 0b000010000011111);
			}
			else if (currentPixel >= 280 && currentPixel <= 304) {
				v = (v & ~(0b0111101111100000)) | (t & 0b0111101111100000);
			}

			if (currentPixel == 1) {
				EndVB();
			}
		}

		currentPixel++;
		if (currentPixel >= scanlineCycles) {
			currentPixel = 0;
			currentScanline++;
			if (currentScanline >= scanlineCount) {
				currentScanline = 0;
			}
		}
	}
}

void PPU::DrawPixel() {
	uint8_t bgPixel = 0, bgColorSet = 0;
	if (bgEnabled && !(!bgShowLeft && currentPixel < 8)) {
		uint8_t tileX = v & 0b11111;
		uint8_t tileY = (v >> 5) & 0b11111;
		uint8_t bgCol = (currentPixel % 8) + x;

		if (bgCol >= 8) {
			bgCol -= 8;
			bgPixel = ((nextTile >> (7 - bgCol)) & 0b1) | (((nextTile >> (14 - bgCol))) & 0b10);
			bgColorSet = nextTileParams;
		}
		else {
			bgPixel = ((currentTile >> (7 - bgCol)) & 0b1) | (((currentTile >> (14 - bgCol))) & 0b10);
			bgColorSet = currentTileParams;
		}
	}

	uint8_t sprPixel = 0, sprColorSet = 0, sprPriority = 0, sprIndex = 0xff;
	if (sprEnabled && sprShowLeft && currentPixel >= 8) {
		for (uint32_t i = 0; i < scanlineSprites.size(); i++) {
			const OAMEntry& spr = scanlineSprites[i];
			if (currentPixel < spr.x || currentPixel >= spr.x + 8) continue;
			uint8_t sprCol = currentPixel - spr.x;
			uint8_t sprRow = currentScanline - spr.y - 1;

			uint8_t sprFetchedPixel = 0;
			if (!sprSize && currentScanline <= spr.y + 8) {
				sprFetchedPixel = GetPixel(sprPattern, spr.tile, sprCol, sprRow, spr.flipX, spr.flipY);
			}
			else if (sprSize && currentScanline <= spr.y + 16) {
				uint8_t tileIndex = spr.bigIndex << 1;
				if (sprRow >= 8) {
					sprRow -= 8;
					tileIndex += 1;
				}
				sprFetchedPixel = GetPixel(spr.bank, tileIndex, sprCol, sprRow, spr.flipX, spr.flipY);
			}

			if (!sprFetchedPixel) continue;
			sprPixel = sprFetchedPixel;
			sprColorSet = spr.palette;
			sprPriority = spr.priority;
			sprIndex = i;
			break;
		}
	}

	Color bgColor = bgPixel ? GetBGColor((bgColorSet << 2) | bgPixel) : GetUniversalBGColor();
	Color sprColor = GetSPRColor((sprColorSet << 2) | sprPixel);
	Color pixel = bgColor;
	if ((sprPixel && !bgPixel) || (sprPixel && !sprPriority)) {
		pixel = sprColor;
	}

	screenTexture.SetPixel(currentPixel, currentScanline - 8, pixel);

	if (spr0Evaluated && sprIndex == 0x00 && currentScanline < 239 && sprPixel && bgPixel && currentPixel != 255 && !(bgShowLeft && sprShowLeft && x < 8)) {
		spr0Hit = true;
	}
}

void PPU::StartVB() {
	verticalBlank = 1;
	PresentFrame();
	NMI();
}

void PPU::EndVB() {
	verticalBlank = 0;
	spr0Hit = 0;
	sprOverflow = 0;
	nmiTriggered = false;
}

void PPU::NMI() {
	if (!nmiTriggered && nmiEnabled && verticalBlank) {
		nmiTriggered = true;
		bus.TriggerNMI();
	}
}

Color PPU::GetSPRColor(uint8_t index) {
	uint8_t colorIndex = ReadSPRPalette(index);
	return GetColorFromPalette(colorIndex);
}

Color PPU::GetBGColor(uint8_t index) {
	uint8_t colorIndex = ReadBGPalette(index);
	return GetColorFromPalette(colorIndex);
}

uint8_t PPU::ReadSPRPalette(uint8_t offset) {
	return bus.ReadVRAM(0x3f10 + offset);
}

uint8_t PPU::ReadBGPalette(uint8_t offset) {
	return bus.ReadVRAM(0x3f00 + offset);
}

Color PPU::GetUniversalBGColor() {
	uint8_t colorIndex = ReadBGPalette(0);
	return GetColorFromPalette(colorIndex);
}

void PPU::EvaluateSprites() {
	scanlineSprites.clear();
	OAMEntry* oamBuffer = bus.GetOAMBuffer();
	for (int32_t i = 0; i < 64; i++) {
		if (oamBuffer[i].y > currentScanline) continue;
		if ((!sprSize && currentScanline < oamBuffer[i].y + 8) ||
			(sprSize && currentScanline < oamBuffer[i].y + 16)) {
			if (i == 0) spr0Evaluated;
			if (scanlineSprites.size() >= spritesPerScanline) {
				sprOverflow = true;
				if(limitScanlineSprites) break;
			}
			scanlineSprites.push_back(oamBuffer[i]);
		}
	}
}

void PPU::incX() {
	FetchTile();
	if ((v & 0x001F) == 31) {
		v &= ~0x001F;
		v ^= 0x0400;
	}
	else {
		v += 1;
	}
}

void PPU::incY() {
	if ((v & 0x7000) != 0x7000) {
		v += 0x1000;
	}
	else {
		v &= ~0x7000;
		int16_t y = (v >> 5) & 0b11111;
		if (y == 29) {
			y = 0;
			v ^= 0x0800;
		}
		else if (y == 31) {
			y = 0;
		}
		else {
			y += 1;
		}
		v = (v & ~0x03E0) | (y << 5);
	}
}

uint8_t PPU::GetTile(uint8_t nametable, uint16_t tileIndex) const {
	uint16_t baseAddress = 0x2000 + 0x400 * (nametable & 0b11);
	return bus.ReadVRAM(baseAddress + tileIndex);
}

uint8_t PPU::GetColorSet(uint8_t nametable, uint8_t tileX, uint8_t tileY) const {
	uint16_t baseAddress = 0x23c0 + 0x400 * (nametable & 0b11);
	uint8_t colorSet = bus.ReadVRAM(baseAddress + ((tileY / 4) * 8 + tileX / 4));
	colorSet = (colorSet >> ((((tileX % 4) / 2) | (((tileY % 4) / 2) << 1)) * 2)) & 0b11;
	return colorSet;
}

uint8_t PPU::GetPixel(uint8_t patternTable, uint8_t tile, uint8_t col, uint8_t row, bool hMirror, bool vMirror) const {
	uint16_t baseAddress = 0x1000 * (patternTable & 0b1);
	uint16_t ind = (tile * 16) + (vMirror ? 7 - row : row);
	if (!hMirror) col = 7 - col;
	uint8_t b1 = (bus.ReadVRAM(baseAddress + ind) >> col) & 1;
	uint8_t b2 = (bus.ReadVRAM(baseAddress + ind + 8) >> col) & 1;
	return (b2 << 1) | b1;
}

Color PPU::GetColorFromPalette(uint8_t index) {
	return palette[index % palette.size()];
}

void PPU::FetchTile() {
	currentTile = nextTile;
	uint16_t tileAddress = 0x2000 | (v & 0xfff);
	uint16_t tile = bus.ReadVRAM(tileAddress);
	uint16_t baseAddress = 0x1000 * bgPattern;
	uint16_t offset = (tile * 16) + ((v >> 12) & 0b111);
	uint16_t address = baseAddress + offset;
	uint8_t b1 = bus.ReadVRAM(address);
	uint8_t b2 = bus.ReadVRAM(address + 8);
	nextTile = (b2 << 8) | b1;

	uint8_t tileX = v & 0b11;
	uint8_t tileY = (v >> 5) & 0b11;
	uint8_t attrOffset = ((tileX >> 1) | (tileY & 0b10)) * 2;

	currentTileParams = nextTileParams;
	uint16_t attributeAddress = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
	uint8_t colorSet = bus.ReadVRAM(attributeAddress);
	colorSet = (colorSet >> attrOffset) & 0b11;
	nextTileParams = colorSet;
}

void PPU::DrawCharPages() {
	for (int32_t charPageIndex = 0; charPageIndex < 4; charPageIndex++) {
		int32_t offsetX = (charPageIndex & 1) * screenWidth;
		int32_t offsetY = ((charPageIndex >> 1) & 1) * charPageHeight;
		for (int32_t y = 0; y < charPageHeight; y++) {
			for (int32_t x = 0; x < screenWidth; x++) {
				uint8_t tileIndex = GetTile(charPageIndex, (y / 8) * 32 + x / 8);
				uint32_t pixel = GetPixel(bgPattern, tileIndex, x % 8, y % 8);
				uint8_t tileX = x / 8;
				uint8_t tileY = (y / 8) + 1;
				uint8_t curColorSet = GetColorSet(charPageIndex, tileX, tileY);
				uint8_t pallete = ReadBGPalette((curColorSet << 2) | pixel);
				Color color = GetColorFromPalette(pallete);
				charPagesTexture.SetPixel(x + offsetX, y + offsetY, color);
			}
		}
	}
}

void PPU::DrawPatternTables() {
	for (int32_t patternTableIndex = 0; patternTableIndex < 2; patternTableIndex++) {
		int32_t offsetX = patternTableIndex * screenWidth;
		for (int32_t row = 0, tile = 0; row < 16; row++) {
			for (int32_t col = 0; col < 16; col++, tile++) {
				for (int32_t y = 0; y < 8; y++) {
					for (int32_t x = 0; x < 8; x++) {
						uint8_t pixel = GetPixel(patternTableIndex, tile, x, y);
						Color color = GetColorFromPalette(pixel);
						patternTablesTexture.SetPixel((col * 8 + x) + offsetX, (row * 8 + y), color);
					}
				}
			}
		}
	}
}

void PPU::DrawSprites() {
	for (int32_t i = 0; i < 0x100; i += 4) {
		OAMEntry spr = bus.GetOAMEntry(i);
		for (int32_t row = 0; row < 8; row++) {
			for (int32_t col = 0; col < 8; col++) {
				uint8_t pixel = GetPixel(sprPattern, spr.tile, col, row, spr.flipX, spr.flipY);
				Color color = GetSPRColor((spr.palette << 2) | pixel);
				screenTexture.SetPixel(spr.x + col, spr.y + row - 8, color);
			}
		}
	}
}