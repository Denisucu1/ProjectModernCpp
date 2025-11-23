#include "Game.h"

Game::Game(std::vector<Player> players) : m_players(players)
{
    m_currentPlayerIndex = 0;

    // Pornim jocul imediat ce este creat
    StartGame();
}

void Game::StartGame()
{
    int cardsPerPlayer = 6; // Default pentru 4-5 jucatori
    if (m_players.size() == 2) {
        cardsPerPlayer = 8; // Regula: 2 jucatori = 8 carti
    }
    else if (m_players.size() == 3) {
        cardsPerPlayer = 7; // Regula: 3 jucatori = 7 carti
    }
}

void Game::GameStatus()
{
}

void Game::GameEndConditions()
{
}

void Game::nextPlayer()
{
}
