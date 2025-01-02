#include "memory.h"

#include <tlhelp32.h>
#include <iostream>
#include <windows.h>

bool MemoryManager::Read(uintptr_t address, void* buffer, size_t size) {
    return ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), buffer, size, nullptr);
}

bool MemoryManager::Write(uintptr_t address, const void* buffer, size_t size) {
    return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(address), buffer, size, nullptr);
}

// Retrieve Process ID by Process Name
DWORD GetProcessIdByName(const char* processName) {
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "[ERROR] Failed to create process snapshot." << std::endl;
        return 0;
    }

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &processEntry)) {
        do {
            if (_wcsicmp(processEntry.szExeFile, std::wstring(processName, processName + strlen(processName)).c_str()) == 0) {
                processId = processEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &processEntry));
    }
    else {
        std::cerr << "[ERROR] Failed to enumerate processes." << std::endl;
    }

    CloseHandle(snapshot);
    return processId;
}

// Retrieve Module Base Address by Module Name
uintptr_t GetModuleBaseAddress(DWORD processId, const wchar_t* moduleName) {
    uintptr_t moduleBaseAddress = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "[ERROR] Failed to create module snapshot." << std::endl;
        return 0;
    }

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(snapshot, &moduleEntry)) {
        do {
            if (_wcsicmp(moduleEntry.szModule, moduleName) == 0) {
                moduleBaseAddress = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
                break;
            }
        } while (Module32Next(snapshot, &moduleEntry));
    }
    else {
        std::cerr << "[ERROR] Failed to enumerate modules." << std::endl;
    }

    CloseHandle(snapshot);
    return moduleBaseAddress;
}



// Attach Function Implementation
bool MemoryManager::Attach(const char* processName) {
    DWORD processId = GetProcessIdByName(processName);
    if (!processId) {
        std::cerr << "[ERROR] Could not find process: " << processName << std::endl;
        return false;
    }

    hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, processId);
    if (!hProcess) {
        std::cerr << "[ERROR] Failed to open process: " << processName << " (Error: " << GetLastError() << ")" << std::endl;
        return false;
    }

    clientDLLBase = GetModuleBaseAddress(processId, L"client.dll");
    engineDLLBase = GetModuleBaseAddress(processId, L"engine.dll");

    if (!clientDLLBase) {
        std::cerr << "[ERROR] Failed to retrieve base address for client.dll." << std::endl;
        CloseHandle(hProcess);
        return false;
    }
    if (!engineDLLBase) {
        std::cerr << "[ERROR] Failed to retrieve base address for engine.dll." << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    std::cout << "[INFO] Attached to process successfully." << std::endl;
    std::cout << "[INFO] client.dll Base Address: 0x" << std::hex << clientDLLBase << std::endl;
    std::cout << "[INFO] engine.dll Base Address: 0x" << std::hex << engineDLLBase << std::endl;

    return true;
}