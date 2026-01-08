#include "pch.h"
#include "Logger.h"
#include <thread>

void MainThread() {
    Logger::Info("DLL successfully injected into The Game!");

    while (true) {
        if (GetAsyncKeyState(VK_F9) & 1) { // F9 pentru a inchide DLL ul
            Logger::Warn("Download DLL...");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr));
    }
    return TRUE;
}