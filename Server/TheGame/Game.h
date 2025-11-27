#pragma once
#include "DrawPile.h"
#include "Player.h"
#include "PlayPiles.h"

class Game
{
public:
	Game(std::vector<Player> players);

	void start_game();

	void game_end_conditions();

	void next_player();
private:
	DrawPile m_cards;
	std::vector<Player> m_players;
	PlayPiles m_play_piles_;
	std::uint8_t m_current_player_index_ : 3;
};

