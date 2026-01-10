#pragma once
#include "DrawPile.h"
#include "Player.h"
#include "PlayPiles.h"
#include <span>
#include <vector>

enum class GameState { InProgress, Won, Lost };

struct PlayerMove {
    std::uint8_t card_value;
    PlayPiles::StackIndex stack_index;
};

class Game {
public:

    static constexpr int TWO_PLAYERS_COUNT = 2;
    static constexpr int THREE_PLAYERS_COUNT = 3;
    static constexpr int HAND_SIZE_TWO_PLAYERS = 8;
    static constexpr int HAND_SIZE_THREE_PLAYERS = 7;
    static constexpr int HAND_SIZE_DEFAULT = 6;

    explicit Game(std::vector<Player> players);

    bool PlaySingleCard(int userId, std::uint8_t cardValue, int stackIndex);
    bool EndCurrentTurn(int userId);
    GameState CheckGameState() const;
    bool CanPlayerMakeAtLeastOneMove(int playerIndex) const;

    const PlayPiles& GetPlayPiles() const;
    const DrawPile& GetDrawPile() const;
    size_t GetDeckSize() const;
    std::uint8_t GetMinCardsToPlay() const;
    const std::vector<Player>& GetPlayers() const;

private:
    static constexpr std::uint8_t MIN_CARDS_EMPTY_DECK = 1;
    static constexpr std::uint8_t MIN_CARDS_FULL_DECK = 2;

    DrawPile m_cards;
    std::vector<Player> m_players;
    PlayPiles m_play_piles_;
    std::uint8_t m_current_player_index;
    int m_cardsPlayedThisTurn;

    void StartGame();
    void NextPlayer(int cardsPlayed);
};