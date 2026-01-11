#include "pch.h"
#include "CodeGenerator.h"
#include <random>

extern "C" std::string GenerateRoomCode()
{
    std::string roomCode;
    const int CODE_LEN = 4;
    roomCode.resize(CODE_LEN);

    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(sizeof(alphanum) - 2));

    for (int i = 0; i < CODE_LEN; ++i)
    {
        roomCode[i] = alphanum[dis(gen)];
    }

    return roomCode;
}