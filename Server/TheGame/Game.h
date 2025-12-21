#pragma once
#include "DrawPile.h"
#include "Player.h"
#include "PlayPiles.h"

struct PlayerMove {
	std::uint8_t card_value;
	PlayPiles::StackIndex stack_index;
};

class Game
{
public:
	Game(std::vector<Player> players);

	Game(Game&& other) noexcept;
	Game& operator=(Game&& other) noexcept;


	void StartGame();

	void GameEndConditions();

	bool ProcessTurn(int playerId, const std::vector<PlayerMove>& moves);

	std::uint8_t GetMinCardsToPlay() const;

	void NextPlayer(int cardsPlayed);

	const std::vector<Player>& GetPlayers() const;

	std::uint8_t GetCurrentPlayerIndex() const;

	const PlayPiles& GetPlayPiles() const { return m_play_piles_; }

	const DrawPile& GetDrawPile() const { return m_cards; }

	bool CanPlayerMakeAtLeastOneMove(int playerId);

private:
	DrawPile m_cards;
	std::vector<Player> m_players;
	PlayPiles m_play_piles_;
	std::uint8_t m_current_player_index_ : 3;
};

