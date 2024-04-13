#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

template <typename T>
struct Texture {
	glm::uvec2 resolution = glm::uvec2(0);
	std::vector<T> pixels = std::vector<T>(0);

	Texture(glm::uvec2 _resolution)
		: resolution(_resolution), pixels(_resolution.x * _resolution.y) {}

	int32_t ByteSize() {
		return pixels.size() * sizeof(T);
	}

	T GetPixel(uint32_t x, uint32_t y) {
		if (x >= resolution.x || y >= resolution.y) return T();
		uint32_t pixelIndex = y * resolution.x + x;
		return pixels[pixelIndex];
	}

	void SetPixel(uint32_t x, uint32_t y, T p) {
		if (x >= resolution.x || y >= resolution.y) return;
		uint32_t pixelIndex = y * resolution.x + x;
		pixels[pixelIndex] = p;
	}

	void AccumulatePixel(uint32_t x, uint32_t y, T p) {
		if (x >= resolution.x || y >= resolution.y) return;
		uint32_t pixelIndex = y * resolution.x + x;
		pixels[pixelIndex] += p;
	}

	void Resize(glm::ivec2 _resolution) {
		resolution = _resolution;
		pixels.resize(resolution.x * resolution.y);
		Reset();
	}

	void Reset() {
		memset(&pixels[0], 0, ByteSize());
	}
};
