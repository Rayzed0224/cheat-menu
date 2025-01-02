#pragma once
#include <Windows.h>
#include <iostream>

class MemoryManager {
public:

    bool Attach(const char* processName);
    bool Read(uintptr_t address, void* buffer, size_t size);
    bool Write(uintptr_t address, const void* buffer, size_t size);
    
  //DWORD64 GetModuleBaseAddress(DWORD processID, const wchar_t* moduleName);

    template <typename T>
    bool Read(DWORD64 address, T* buffer, SIZE_T size) {
        SIZE_T bytesRead;
        return ReadProcessMemory(hProcess, (LPCVOID)address, buffer, size, &bytesRead);
    }

    DWORD64 clientDLLBase = 0;         // Base address of client.dll
    DWORD64 engineDLLBase = 0;         // Base address of engine.dll

private:
    HANDLE hProcess = nullptr;
};
