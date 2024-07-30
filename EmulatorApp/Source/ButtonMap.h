#pragma once

#include "GLFW/glfw3.h"
#include <array>
#include <string>

enum class EmulatorButton {
	A = 0,
	B,
	Select,
	Start,
	Right,
	Left,
	Up,
	Down,
	Reset,
	Pause,
	Resume,
	Step,
	COUNT
};

struct ButtonMap {
	std::array<uint32_t, (uint32_t)EmulatorButton::COUNT> mappings = {
		GLFW_KEY_C,
		GLFW_KEY_V,
		GLFW_KEY_Z,
		GLFW_KEY_X,
		GLFW_KEY_RIGHT,
		GLFW_KEY_LEFT,
		GLFW_KEY_UP,
		GLFW_KEY_DOWN,
		GLFW_KEY_R,
		GLFW_KEY_ENTER,
		GLFW_KEY_ENTER,
		GLFW_KEY_SPACE
	};
};

std::string GLFWKeyToString(uint32_t key);