#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <PixieUI/PixieUI.h>
#include "NESEmulator.h"
#include "ScreenPlane.h"
#include "TextureUploader.h"

class EmulatorWindow {
public:
	GLFWwindow* mainWindow = nullptr;

	EmulatorWindow(uint32_t width, uint32_t height);
	~EmulatorWindow();

	bool LoadROM(const std::string& romPath);
	void Start();
	void SetResolution(uint32_t width, uint32_t height);

private:
	NesEmulator emulator;
	ScreenPlane* screenPlane = nullptr;
	TextureUploader<Color>* screenUploader = nullptr;
	TextureUploader<Color>* charPagesUploader = nullptr;
	TextureUploader<Color>* patternTablesUploader = nullptr;
	uint32_t width;
	uint32_t height;

	void UpdateKeyStates();
};

