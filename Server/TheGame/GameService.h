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

struct MatchPlayerData {
	int id;             
	int userId;
	std::vector<int> cardsInHand;
};

struct MatchData {
	int matchId;
	std::string status;
	int currentTurnPlayerId;
	int deckCount;
	std::vector<int> stacks;
	std::vector<MatchPlayerData> players;
};

class GameService
{
public:
	GameService();

	std::optional<GameId> FindGame(UserId userId, const std::string& username, int desiredPlayerCount);

	std::optional<GameId> GetPlayerGameStatus(UserId userId);

	Game& GetGame(const GameId gameId);

	std::optional<MatchData> GetMatchState(int matchIdInt);
private:
	std::mutex m_mutex;

	struct WaitingPlayer
	{
		UserId id;
		std::string name;
		std::chrono::steady_clock::time_point joinTime;
	};

	std::map<int, std::list<WaitingPlayer>> m_waitingQueues;

	std::map<GameId, Game> m_activeGames;

	std::map<UserId, int> m_playerInQueueMap;

	std::map<UserId, GameId> m_playerGameMap;

	void CreateGame(std::list<WaitingPlayer>& players, int playerCount);

	long long m_gameIdCounter = 0;
};

