#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <PixieUI/PixieUI.h>
#include "NESEmulator.h"
#include "ScreenPlane.h"
#include "TextureUploader.h"

class EmulatorWindow {
public:
	GLFWwindow* mainWindow = nullptr;

	EmulatorWindow(glm::ivec2 resolution);
	~EmulatorWindow();

	bool LoadROM(const std::string& romPath);
	void Start();
	void SetResolution(glm::ivec2 resolution);

private:
	ScreenPlane* screenPlane = nullptr;
	TextureUploader<Color>* screenUploader = nullptr;
	TextureUploader<Color>* charPagesUploader = nullptr;
	TextureUploader<Color>* patternTablesUploader = nullptr;
	glm::ivec2 resolution = glm::ivec2(0, 0);
	NesEmulator emulator;

	void UpdateKeyStates();
};

