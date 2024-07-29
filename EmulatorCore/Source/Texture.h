#pragma once
#include <cstdint>
#include <iostream>
#include <vector>

template <typename T>
struct Texture {
	uint32_t width;
	uint32_t height;
	std::vector<T> pixels = std::vector<T>(0);

	Texture(uint32_t width, uint32_t height)
		: width(width), height(height), pixels(width * height) {}

	int32_t ByteSize() {
		return pixels.size() * sizeof(T);
	}

	T GetPixel(uint32_t x, uint32_t y) {
		if (x >= width || y >= height) return T();
		uint32_t pixelIndex = y * width + x;
		return pixels[pixelIndex];
	}

	void SetPixel(uint32_t x, uint32_t y, T p) {
		if (x >= width || y >= height) return;
		uint32_t pixelIndex = y * width + x;
		pixels[pixelIndex] = p;
	}

	void AccumulatePixel(uint32_t x, uint32_t y, T p) {
		if (x >= width || y >= height) return;
		uint32_t pixelIndex = y * width + x;
		pixels[pixelIndex] += p;
	}

	void Resize(uint32_t _width, uint32_t _height) {
		width = _width;
		height = _height;
		pixels.resize(width * height);
		Reset();
	}

	void Reset() {
		memset(&pixels[0], 0, ByteSize());
	}
};
