#pragma once

#include <Windows.h>

namespace offsets
{
	inline DWORD64 dwViewMatrix = 0x1A7F610;
	inline DWORD64 dwEntityList = 0x1A146E8;
	inline DWORD64 dwLocalPlayer = 0x1868CC8;
	inline DWORD64 m_iTeamNum = 0x3E3;
	inline DWORD64 m_iHealth = 0x344;
	inline DWORD64 m_vecOrigin = 0x88;
	inline DWORD64 dwPlantedC4 = 0x1A84F90;
}

namespace globals
{
	// Screen dimensions
	constexpr int SCREEN_WIDTH = 1920;
	constexpr int SCREEN_HEIGHT = 1080;
	inline bool enableESP = false;
	inline bool isMenuVisible = true;
}
