#include "pch.h"
#include "Logger.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <Windows.h>

std::string Logger::GetTimestamp() {
    auto acum = std::chrono::system_clock::now();
    auto timp_t = std::chrono::system_clock::to_time_t(acum);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(acum.time_since_epoch()) % 1000;

    std::tm bt;
    localtime_s(&bt, &timp_t);

    std::ostringstream oss;
    oss << std::put_time(&bt, "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string Logger::LevelToString(Level nivel) {
    switch (nivel) {
    case Level::Info:    return "INFO";
    case Level::Warning: return "WARN";
    case Level::Error:   return "ERR ";
    default:             return "NONE";
    }
}

void Logger::Log(const std::string& mesaj, Level nivel) {
    std::lock_guard<std::mutex> lock(logMutex);

    std::string formatat = "[" + GetTimestamp() + "] [" + LevelToString(nivel) + "] " + mesaj + "\n";

    std::ofstream file("thegame_log.txt", std::ios::app);
    if (file.is_open()) {
        file << formatat;
        file.close();
    }

    OutputDebugStringA(formatat.c_str());

    printf("%s", formatat.c_str());
}