#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <PixieUI/PixieUI.h>
#include <filesystem>
#include "ButtonMap.h"
#include "NESEmulator.h"
#include "EmulatorWindowUI.h"

class EmulatorWindowUI;

enum class FileAccessState {
	Ok = 0,
	CouldNotOpenFile,
	FileDoesntExist
};

class EmulatorWindow {
public:
	GLFWwindow* m_mainWindow = nullptr;

	EmulatorWindow(uint32_t width, uint32_t height);
	~EmulatorWindow();

	void UpdateScreen();
	bool LoadROM(const std::string& romPath);
	void Start();
	bool IsPaused();

	void HandleResolutionChange(uint32_t width, uint32_t height);
	void HandleMouseEvent(int32_t button, int32_t action);
	void HandleMouseMove(double x, double y);
	void HandleKeyEvent(int32_t key, int32_t scancode, int32_t action);

protected:
	NesEmulator m_emulator;
	uint32_t m_width;
	uint32_t m_height;
	double m_targetFPS = 1000.0;
	double m_targetFrameTime = 1.0 / m_targetFPS;
	ButtonMap m_buttonMap;
	PixieUI::FrameBuffer* m_layoutFrameBuffer;
	EmulatorWindowUI* m_ui;
	bool m_isRebindingKey = false;
	EmulatorButton m_keyToRebind = EmulatorButton::A;
	bool m_paused = false;

	void UpdateKeyStates();
	bool ButtonIsPressed(EmulatorButton button);

	FileAccessState LoadSettings();
	FileAccessState SaveSettings();
	FileAccessState LoadState();
	FileAccessState SaveState();

	friend class EmulatorWindowUI;
};

