#include "EmulatorWindow.h"

EmulatorWindow emulatorWindow(512, 448);

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
	emulatorWindow.HandleResolutionChange(width, height);
}

void DropCallback(GLFWwindow* window, int count, const char** paths) {
	emulatorWindow.LoadROM(paths[0]);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	emulatorWindow.HandleMouseEvent(button, action);
}

void CursorPosCallback(GLFWwindow* window, double x, double y) {
	emulatorWindow.HandleMouseMove(x, y);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int modes) {
	emulatorWindow.HandleKeyEvent(key, scancode, action);
}

int main(int argc, char** argv) {
	std::ios_base::sync_with_stdio(false);
	std::cout << std::hex;

	glfwSetWindowSizeCallback(emulatorWindow.m_mainWindow, WindowSizeCallback);
	glfwSetDropCallback(emulatorWindow.m_mainWindow, DropCallback);
	glfwSetMouseButtonCallback(emulatorWindow.m_mainWindow, MouseButtonCallback);
	glfwSetCursorPosCallback(emulatorWindow.m_mainWindow, CursorPosCallback);
	glfwSetKeyCallback(emulatorWindow.m_mainWindow, KeyCallback);

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