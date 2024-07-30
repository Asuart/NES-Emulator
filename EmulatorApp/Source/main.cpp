#include "EmulatorWindow.h"

EmulatorWindow emulatorWindow(512, 448);

void drop_callback(GLFWwindow* window, int count, const char** paths) {
	emulatorWindow.LoadROM(paths[0]);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	emulatorWindow.SetResolution(width, height);
	glViewport(0, 0, width, height);
}

int main(int argc, char** argv) {
	std::ios_base::sync_with_stdio(false);
	std::cout << std::hex;

	glfwSetWindowSizeCallback(emulatorWindow.m_mainWindow, window_size_callback);
	glfwSetDropCallback(emulatorWindow.m_mainWindow, drop_callback);

	if (argc > 2) {
		std::cout << "Programm uses first argument as a path to a ROM file. All other are ignored.\n";
	}
	std::cout << "Emulator can run some NMAP ROMs. SPU is not implemented. Sprite Overflow is not implemented. Timings are far from great.\n";
	std::cout << "You can drag and drop ROM-files on window\n";
	std::cout << "Controls: Arrows, Z - select, X - start, C - a, V - b\n";
	std::cout << "\n";

	if (argc > 1) {
		emulatorWindow.LoadROM(argv[1]);
	}

	emulatorWindow.Start();

	return 0;
}