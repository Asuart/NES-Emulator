#pragma once
#include "Globals.h"
#include "Texture.h"
#include "MMC.h"
#include "CPU.h"
#include "Bus.h"

class Bus;

class PPU {
public:
	static const uint32_t screenWidth = 256;
	static const uint32_t screenHeight = 224;

private:
	static const uint32_t charPageHeight = 240;
	static const uint32_t patterTableSize = 256;
	static const uint32_t scanlineCount = 262;
	static const uint32_t scanlineCycles = 341;

	const float cpuClockRatio = 3;

	Texture<Color> screenTexture = Texture<Color>(glm::ivec2(screenWidth, screenHeight));
	Texture<Color> charPagesTexture = Texture<Color>(glm::ivec2(screenWidth * 2, charPageHeight * 2));
	Texture<Color> patternTablesTexture = Texture<Color>(glm::ivec2(patterTableSize * 2, patterTableSize));
	const DisplayMode displayMode = DisplayMode::NTSC;

	Bus* bus;

#pragma pack(push, 1)
	union {
		uint8_t ctrl;
		struct {
			uint8_t nametable : 2;
			uint8_t increment : 1;
			uint8_t sprPattern : 1;
			uint8_t bgPattern : 1;
			uint8_t sprSize : 1;
			uint8_t mode : 1;
			uint8_t nmiEnabled : 1;
		};
	};

	union {
		uint8_t mask;
		struct {
			uint8_t greyscale : 1;
			uint8_t bgShowLeft : 1;
			uint8_t sprShowLeft : 1;
			uint8_t bgEnabled : 1;
			uint8_t sprEnabled : 1;
			uint8_t eRed : 1;
			uint8_t eGreen : 1;
			uint8_t eBlue : 1;
		};
	};

	union {
		uint8_t status;
		struct {
			uint8_t openBus : 5;
			uint8_t sprOverflow : 1;
			uint8_t spr0Hit : 1;
			uint8_t verticalBlank : 1;
		};
	};
#pragma pack(pop)

	uint8_t scroll;
	uint8_t address;
	uint8_t data, dataReadBuffer;
	uint8_t oamAddress;
	uint8_t oamData;

	uint16_t currentScanline, currentPixel;
	uint16_t vramAddress, tempVramAddress;
	uint16_t currentTile, nextTile;
	uint8_t currentTileParams, nextTileParams;

	uint16_t v, t;
	uint8_t x, w;

	bool nmiTriggered = false;

public:
	bool frameReady = false;

	PPU(Bus* interconnect);

	void Reset();
	void Step();

	void DrawPixel();
	void PresentFrame();

	uint8_t ReadRegister(uint8_t index, bool fetch = false);
	void WriteRegister(uint8_t index, uint8_t value);

private:
	void StartVB();
	void EndVB();
	void NMI();

	void incX();
	void incY();
	void FetchTile();

	Color GetSPRColor(uint8_t index);
	Color GetBGColor(uint8_t index);
	uint8_t ReadSPRPalette(uint8_t offset);
	uint8_t ReadBGPalette(uint8_t offset);
	Color GetUniversalBGColor();
	void EvaluateSpriteOverflow();

	uint8_t GetTile(uint8_t nametable, uint16_t tileIndex) const;
	uint8_t GetColorSet(uint8_t nametable, uint8_t tileX, uint8_t tileY) const;
	uint8_t GetPixel(uint8_t patternTable, uint8_t tile, uint8_t col, uint8_t row, bool hMirror = false, bool vMirror = false) const;
	Color GetColorFromPalette(uint8_t index);

	void DrawCharPages();
	void DrawPatternTables();
	void DrawSprites();

	std::array<glm::vec3, 0x40> palette = {
		// 0x0X
		Color(117, 117, 117),
		Color(39, 27, 143),
		Color(0, 0, 171),
		Color(71, 0, 159),
		Color(143, 0, 119),
		Color(171, 0, 19),
		Color(167, 0, 0),
		Color(127, 11, 0),
		Color(67, 47, 0),
		Color(0, 71, 0),
		Color(0, 81, 0),
		Color(0, 63, 23),
		Color(27, 63, 95),
		Color(0, 0, 0),
		Color(0, 0, 0),
		Color(0, 0, 0),
		// 0x1X
		Color(188, 188, 188),
		Color(0, 115, 239),
		Color(35, 59, 239),
		Color(131, 0, 243),
		Color(191, 0, 191),
		Color(231, 0, 91),
		Color(219, 43, 0),
		Color(203, 79, 15),
		Color(139, 115, 0),
		Color(0, 151, 0),
		Color(0, 171, 0),
		Color(0, 147, 59),
		Color(0, 131, 139),
		Color(0, 0, 0),
		Color(0, 0, 0),
		Color(0, 0, 0),
		// 0x2X
		Color(255, 255, 255),
		Color(63, 191, 255),
		Color(95, 151, 255),
		Color(167, 139, 253),
		Color(247, 123, 255),
		Color(255, 119, 183),
		Color(255, 119, 99),
		Color(255, 155, 59),
		Color(243, 191, 63),
		Color(131, 211, 19),
		Color(79, 223, 75),
		Color(88, 248, 152),
		Color(0, 235, 219),
		Color(0, 0, 0),
		Color(0, 0, 0),
		Color(0, 0, 0),
		// 0x3X
		Color(255, 255, 255),
		Color(171, 231, 255),
		Color(199, 215, 255),
		Color(215, 203, 255),
		Color(255, 199, 255),
		Color(255, 199, 219),
		Color(255, 191, 179),
		Color(255, 219, 171),
		Color(255, 231, 163),
		Color(227, 255, 163),
		Color(171, 243, 191),
		Color(179, 255, 207),
		Color(159, 255, 243),
		Color(0, 0, 0),
		Color(0, 0, 0),
		Color(0, 0, 0),
	};

	friend class EmulatorWindow;
	friend class NesEmulator;
	friend class MMC;
};

