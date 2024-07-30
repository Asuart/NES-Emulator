#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <atomic>

enum class DisplayMode : uint32_t {
	NTSC = 0,
	PAL
};

enum class MirroringMode : uint32_t {
	PAGE0 = 0,
	PAGE1,
	VERTICAL,
	HORIZONTAL,
	FULL
};

const int32_t prgBankSize = 16384;
const int32_t chrBankSize = 8192;

const uint32_t screenWidth = 256;
const uint32_t screenHeight = 224;
const uint32_t scanlineCount = 262;
const uint32_t scanlineCycles = 341;
const uint32_t charPageHeight = 240;
const uint32_t patterTableSize = 256;

struct RomHeader;
struct OAMEntry;

class NesEmulator;
class Bus;
class CPU;
class MMC;
class PPU;
class IO;
class APU;
class MMC;
class NMAP;
class MMC1;