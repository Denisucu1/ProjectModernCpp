#pragma once
#include <string>
#include <mutex>

#ifdef LOGGER_EXPORTS
#define LOGGER_API __declspec(dllexport)
#else
#define LOGGER_API __declspec(dllimport)
#endif

class LOGGER_API Logger {
public:
    enum class Level { Info, Warning, Error };

    static void Log(const std::string& mesaj, Level nivel = Level::Info);

    static void Info(const std::string& m) { Log(m, Level::Info); }
    static void Warn(const std::string& m) { Log(m, Level::Warning); }
    static void Error(const std::string& m) { Log(m, Level::Error); }

private:
    static std::string GetTimestamp();
    static std::string LevelToString(Level nivel);
    static std::mutex logMutex;
};