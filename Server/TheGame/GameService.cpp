#include "GameService.h"
#include "crow.h"
#include <iostream>

GameService::GameService()
{
    m_waitingQueues[2] = std::list<WaitingPlayer>();
    m_waitingQueues[3] = std::list<WaitingPlayer>();
    m_waitingQueues[4] = std::list<WaitingPlayer>();
    m_waitingQueues[5] = std::list<WaitingPlayer>();
}

std::optional<GameId> GameService::FindGame(UserId userId, const std::string& username, int desiredPlayerCount)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_playerGameMap.count(userId))
    {
        return m_playerGameMap[userId];
    }

    if (m_playerInQueueMap.count(userId))
    {
        return std::nullopt;
    }

    std::cout << "[GameService] Player " << username << "(ID: " << userId
        << ") queues for a " << desiredPlayerCount << "-player game." << std::endl;

    m_waitingQueues[desiredPlayerCount].push_back({ userId, username, std::chrono::steady_clock::now() });
    m_playerInQueueMap[userId] = desiredPlayerCount;

    auto& queue = m_waitingQueues[desiredPlayerCount];
    auto now = std::chrono::steady_clock::now();
    auto oldestPlayerTime = queue.empty() ? now : queue.front().joinTime;
    auto timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - oldestPlayerTime).count();

    bool isGameReady = false;

    if (queue.size() >= desiredPlayerCount)
    {
        std::cout << "[GameService] Queue for " << desiredPlayerCount << " is full. Starting game." << std::endl;
        isGameReady = true;
    }
    else if (timeElapsed > 30 && queue.size() >= 2) {
        std::cout << "[GameService] Starting game for queue " << desiredPlayerCount
            << " due to 30s timeout with " << queue.size() << " players." << std::endl;
        isGameReady = true;
    }

    if (isGameReady)
    {
        //
    }
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
