#include "EmulatorWindow.h"

EmulatorWindow::EmulatorWindow(uint32_t width, uint32_t height)
	: m_width(width), m_height(height), m_layout(width, height) {
	if (glfwInit() != GL_TRUE) {
		std::cout << "GLFW failed to initialize\n";
		exit(1);
	}

	m_mainWindow = glfwCreateWindow(width, height, "NES Emulator", NULL, NULL);
	glfwMakeContextCurrent(m_mainWindow);
	glfwSetInputMode(m_mainWindow, GLFW_STICKY_KEYS, GL_TRUE);

	if (!gladLoadGL()) {
		std::cout << "GLAD failed to initialize\n";
		exit(2);
	}

	PixieUI::Init();
	PixieUI::SetCanvasSize(m_emulator.ppu.screenWidth, m_emulator.ppu.screenHeight);

	m_viewportTexture = new PixieUI::Texture(0, 0, m_emulator.ppu.screenWidth, m_emulator.ppu.screenHeight);
	m_layout.AttachElement(m_viewportTexture);
}

EmulatorWindow::~EmulatorWindow() {
	glfwDestroyWindow(m_mainWindow);
	glfwTerminate();
}

void EmulatorWindow::SetResolution(uint32_t width, uint32_t height) {
	m_width = width;
	m_height = height;
}

bool EmulatorWindow::LoadROM(const std::string& romPath) {
	std::cout << "Loading rom: \"" << romPath << "\"\n";
	std::ifstream reader(romPath, std::ifstream::binary | std::fstream::ate);
	if (!reader) {
		std::cout << "Could not open ROM file: \"" << romPath << "\"\n";
		return false;
	}

	uint32_t romSize = (uint32_t)reader.tellg();
	uint8_t* ROMdata = new uint8_t[romSize];
	reader.seekg(0, reader.beg);
	reader.read((char*)ROMdata, romSize);
	reader.close();

	if (romSize < 16) {
		std::cout << "ROM size is too small: \"" << romPath << "\"\n";
		return false;
	}

	bool loaded = m_emulator.LoadROM(ROMdata, romSize);

	delete[] ROMdata;

	return loaded;
}

void EmulatorWindow::Start() {
	double lastTime = glfwGetTime();
	double timeAccumulator = 0;
	while (!glfwWindowShouldClose(m_mainWindow)) {
		glfwPollEvents();
		UpdateKeyStates();

		m_emulator.Run(256);

		if (m_emulator.ppu.frameReady) {
			m_emulator.ppu.frameReady = false;
			m_viewportTexture->UploadTexture(
				m_emulator.ppu.screenWidth, m_emulator.ppu.screenHeight,
				m_emulator.ppu.screenTexture.pixels.data(),
				GL_RGB, GL_UNSIGNED_BYTE
			);

			double newTime = glfwGetTime();
			timeAccumulator += newTime - lastTime;
			lastTime = newTime;
			if (timeAccumulator < m_targetFrameTime) {
				glfwWaitEventsTimeout(m_targetFrameTime - timeAccumulator);
			}
			std::string newTitle = "NES Emulator: " + std::to_string(timeAccumulator * 1000) + "ms";
			glfwSetWindowTitle(m_mainWindow, newTitle.c_str());
			timeAccumulator = 0;

			glClear(GL_COLOR_BUFFER_BIT);

			m_layout.Draw();

			glfwSwapBuffers(m_mainWindow);

		}
	}
}

void EmulatorWindow::UpdateKeyStates() {
	m_emulator.io.keyStates[0] = glfwGetKey(m_mainWindow, GLFW_KEY_C) > 0 ? 1 : 0;
	m_emulator.io.keyStates[1] = glfwGetKey(m_mainWindow, GLFW_KEY_V) > 0 ? 1 : 0;
	m_emulator.io.keyStates[2] = glfwGetKey(m_mainWindow, GLFW_KEY_Z) > 0 ? 1 : 0;
	m_emulator.io.keyStates[3] = glfwGetKey(m_mainWindow, GLFW_KEY_X) > 0 ? 1 : 0;
	m_emulator.io.keyStates[4] = glfwGetKey(m_mainWindow, GLFW_KEY_UP) > 0 ? 1 : 0;
	m_emulator.io.keyStates[5] = glfwGetKey(m_mainWindow, GLFW_KEY_DOWN) > 0 ? 1 : 0;
	m_emulator.io.keyStates[6] = glfwGetKey(m_mainWindow, GLFW_KEY_LEFT) > 0 ? 1 : 0;
	m_emulator.io.keyStates[7] = glfwGetKey(m_mainWindow, GLFW_KEY_RIGHT) > 0 ? 1 : 0;
}