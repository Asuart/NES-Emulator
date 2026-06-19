#pragma once
#include <cstdint>

struct Color {
	uint8_t r, g, b;
	Color(int32_t r = 0, int32_t g = 0, int32_t b = 0);
};