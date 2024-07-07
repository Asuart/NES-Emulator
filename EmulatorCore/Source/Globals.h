#pragma once
#include <glm/glm.hpp>
#include <cstdint>
#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <atomic>

enum class DisplayMode {
	NTSC = 0,
	PAL
};

enum class MirroringMode {
	PAGE0 = 0,
	PAGE1,
	VERTICAL,
	HORIZONTAL,
	FULL
};

using Color = glm::i8vec3;

const int32_t prgBankSize = 16384;
const int32_t chrBankSize = 8192;

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