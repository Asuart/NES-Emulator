#include "EmulatorWindow.h"

EmulatorWindow::EmulatorWindow(uint32_t width, uint32_t height)
	: width(width), height(height) {
	if (glfwInit() != GL_TRUE) {
		std::cout << "GLFW failed to initialize\n";
		exit(1);
	}

	mainWindow = glfwCreateWindow(width, height, "NES Emulator", NULL, NULL);
	glfwMakeContextCurrent(mainWindow);
	glfwSetInputMode(mainWindow, GLFW_STICKY_KEYS, GL_TRUE);

	if (!gladLoadGL()) {
		std::cout << "GLAD failed to initialize\n";
		exit(2);
	}

	PixieUI::Init();
	PixieUI::SetCanvasSize(emulator.ppu.screenWidth, emulator.ppu.screenHeight);

	screenPlane = new ScreenPlane();

	screenUploader = new TextureUploader(emulator.ppu.screenTexture);
	charPagesUploader = new TextureUploader(emulator.ppu.charPagesTexture);
	patternTablesUploader = new TextureUploader(emulator.ppu.patternTablesTexture);
}

EmulatorWindow::~EmulatorWindow() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
	delete screenPlane;
	delete screenUploader;
	delete charPagesUploader;
	delete patternTablesUploader;
}

void EmulatorWindow::SetResolution(uint32_t _width, uint32_t _height) {
	width = _width;
	height = _height;
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

	bool loaded = emulator.LoadROM(ROMdata, romSize);

	delete[] ROMdata;

	return loaded;
}

void EmulatorWindow::Start() {
	const double targetFPS = 60;
	const double frameTime = 1.0 / targetFPS;
	double lastTime = glfwGetTime();
	double timeAccumulator = 0;
	while (!glfwWindowShouldClose(mainWindow)) {
		glfwPollEvents();
		UpdateKeyStates();

		emulator.Run(256);

		if (emulator.ppu.frameReady) {
			emulator.ppu.frameReady = false;
			double newTime = glfwGetTime();
			timeAccumulator += newTime - lastTime;
			lastTime = newTime;

			if (timeAccumulator < frameTime) {
				glfwWaitEventsTimeout(frameTime - timeAccumulator);
			}
			timeAccumulator = 0;

			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(screenPlane->shader);

			GLint aspectLoc = glGetUniformLocation(screenPlane->shader, "aspect");
			screenUploader->Upload();
			screenUploader->Bind(GL_TEXTURE0);
			float aspect = ((float)emulator.ppu.screenTexture.width / emulator.ppu.screenTexture.height) / ((float)width / height);
			//emulator.ppu->DrawCharPages();
			//charPagesUploader->Upload();
			//charPagesUploader->Bind(GL_TEXTURE0);
			//float aspect = ((float)emulator.ppu->charPagesTexture.resolution.x / emulator.ppu->charPagesTexture.resolution.y) / ((float)resolution.x / resolution.y);
			glUniform1f(aspectLoc, aspect);
			glBindVertexArray(screenPlane->vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			glfwSwapBuffers(mainWindow);
		}
	}
}

void EmulatorWindow::UpdateKeyStates() {
	emulator.io.keyStates[0] = glfwGetKey(mainWindow, GLFW_KEY_C) > 0 ? 1 : 0;
	emulator.io.keyStates[1] = glfwGetKey(mainWindow, GLFW_KEY_V) > 0 ? 1 : 0;
	emulator.io.keyStates[2] = glfwGetKey(mainWindow, GLFW_KEY_Z) > 0 ? 1 : 0;
	emulator.io.keyStates[3] = glfwGetKey(mainWindow, GLFW_KEY_X) > 0 ? 1 : 0;
	emulator.io.keyStates[4] = glfwGetKey(mainWindow, GLFW_KEY_UP) > 0 ? 1 : 0;
	emulator.io.keyStates[5] = glfwGetKey(mainWindow, GLFW_KEY_DOWN) > 0 ? 1 : 0;
	emulator.io.keyStates[6] = glfwGetKey(mainWindow, GLFW_KEY_LEFT) > 0 ? 1 : 0;
	emulator.io.keyStates[7] = glfwGetKey(mainWindow, GLFW_KEY_RIGHT) > 0 ? 1 : 0;
}