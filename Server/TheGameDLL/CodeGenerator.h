#pragma once
#include <string>

#ifdef CODEGEN_EXPORTS
#define CODEGEN_API __declspec(dllexport)
#else
#define CODEGEN_API __declspec(dllimport)
#endif

extern "C" {
    CODEGEN_API std::string GenerateRoomCode();
}