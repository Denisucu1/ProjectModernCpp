#include "GameService.h"

GameService::GameService()
{
}

std::optional<GameId> GameService::FindGame(UserId userId, const std::string& username, int desiredPlayerCount)
{
    //
}

std::optional<GameId> GameService::GetPlayerGameStatus(UserId userId)
{
    //
}

Game& GameService::GetGame(const GameId gameId)
{
    // TODO: insert return statement here
}

void GameService::CreateGame(std::list<WaitingPlayer>& players, int playerCount)
{
}
