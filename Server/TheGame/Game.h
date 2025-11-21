#pragma once
#include "DrawPile.h"
#include "Player.h"
#include "PlayPiles.h"

class Game
{
public:
	Game(std::vector<Player> players);

	void StartGame();

	void GameStatus();

	void GameEndConditions();

	void nextPlayer();
private:
	DrawPile m_cards;
	std::vector<Player> m_players;
	PlayPiles m_playPiles;
	std::uint8_t m_currentPlayerIndex : 3;
};

