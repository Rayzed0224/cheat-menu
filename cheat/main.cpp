#include "gui.h"
#include "esp.h"
#include "memory.h"

#include "globals.h"

#include <thread>
#include <chrono>

MemoryManager memMgr;
HHOOK keyHook;

void PollKeyInput() {
	static bool isInsertKeyPressed = false;

	// Check if the Insert key is currently pressed
	if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
		if (!isInsertKeyPressed) { // Key press detected
			globals::isMenuVisible = !globals::isMenuVisible;

			// Toggle overlay interactivity
			if (globals::isMenuVisible) {
				SetWindowLong(gui::window, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST);
				SetForegroundWindow(gui::window);
			}
			else {
				SetWindowLong(gui::window, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT);
			}

			std::cout << "[DEBUG] Menu Visibility Toggled: " << globals::isMenuVisible << std::endl;

			isInsertKeyPressed = true;
		}
	}
	else {
		isInsertKeyPressed = false; // Key released
	}
}



int WINAPI wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR pCmdLine,
	int nCmdShow
) {
	
	// Attach to the game process
	if (!memMgr.Attach("cs2.exe")) {
		std::cerr << "[ERROR] Initial attach failed. Retrying..." << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		if (!memMgr.Attach("cs2.exe")) {
			std::cerr << "[ERROR] Failed to attach to CS2 process after retrying." << std::endl;
			return EXIT_FAILURE;
		}
	}

	// Create gui
	gui::CreateHWindow("Cheat Menu");
	gui::CreateDevice();
	gui::CreateImGui();

	esp::Initialize();

	while (gui::isRunning)
	{
		PollKeyInput();
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	UnhookWindowsHookEx(keyHook);

	// Destroy gui
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	return EXIT_SUCCESS;
}
