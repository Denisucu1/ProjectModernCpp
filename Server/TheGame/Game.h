#pragma once
#include "DrawPile.h"
#include "Player.h"
#include "PlayPiles.h"
#include <span>

enum class GameState { InProgress, Won, Lost };

struct PlayerMove {
    std::uint8_t card_value;
    PlayPiles::StackIndex stack_index;
};

class Game {
public:
    explicit Game(std::vector<Player> players);

    bool PlaySingleCard(int userId, std::uint8_t cardValue, int stackIndex);
    bool EndCurrentTurn(int userId);

    GameState CheckGameState() const;
    bool CanPlayerMakeAtLeastOneMove(int playerIndex) const;

    const PlayPiles& GetPlayPiles() const { return m_play_piles_; }
    const DrawPile& GetDrawPile() const { return m_cards; }
    size_t GetDeckSize() const { return m_cards.GetSize(); }
    std::uint8_t GetMinCardsToPlay() const { return m_cards.IsEmpty() ? 1 : 2; }
    const std::vector<Player>& GetPlayers() const { return m_players; }

private:
    DrawPile m_cards;
    std::vector<Player> m_players;
    PlayPiles m_play_piles_;
    std::uint8_t m_current_player_index = 0;
    int m_cardsPlayedThisTurn = 0;

    void StartGame();
    void NextPlayer(int cardsPlayed);
};