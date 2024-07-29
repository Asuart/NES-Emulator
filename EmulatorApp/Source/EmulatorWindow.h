#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <PixieUI/PixieUI.h>
#include "NESEmulator.h"

class EmulatorWindow {
public:
	GLFWwindow* m_mainWindow = nullptr;

	EmulatorWindow(uint32_t width, uint32_t height);
	~EmulatorWindow();

	bool LoadROM(const std::string& romPath);
	void Start();
	void SetResolution(uint32_t width, uint32_t height);

private:
	NesEmulator m_emulator;
	uint32_t m_width;
	uint32_t m_height;
	PixieUI::Layout m_layout;
	PixieUI::Texture* m_viewportTexture;
	double m_targetFPS = 60.0;
	double m_targetFrameTime = 1.0 / m_targetFPS;

	void UpdateKeyStates();
};

