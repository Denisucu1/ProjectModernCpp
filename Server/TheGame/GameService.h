#pragma once
#include <list>
#include <string>
#include <map>
#include <mutex>
#include <optional>
#include <chrono>
#include "Game.h"
#include "Player.h"

using GameId = std::string;
using UserId = int;

class GameService
{
	GameService();

	std::optional<GameId> FindGame(UserId userId, const std::string& username, int desiredPlayerCount);

	std::optional<GameId> GetPlayerGameStatus(UserId userId);

	Game& GetGame(const GameId gameId);
};

