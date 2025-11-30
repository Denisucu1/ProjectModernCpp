#pragma once
#include "DrawPile.h"
#include "Player.h"
#include "PlayPiles.h"

class Game
{
public:
	Game(std::vector<Player> players);

	void StartGame();

	void GameEndConditions();

	void NextPlayer();
private:
	DrawPile m_cards;
	std::vector<Player> m_players;
	PlayPiles m_play_piles_;
	std::uint8_t m_current_player_index_ : 3;
};

