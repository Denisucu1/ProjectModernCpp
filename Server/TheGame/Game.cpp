#include "Game.h"
#include <algorithm>
#include <ranges>

Game::Game(std::vector<Player> players)
    : m_players(std::move(players)), m_cards(), m_play_piles_(), m_current_player_index(0)
{
    StartGame();
}

void Game::refillPlayerHand(Player& player)
{
    int cardsNeeded = (m_players.size() == 2) ? 8 : (m_players.size() == 3 ? 7 : 6);
    auto deckSpan = player.GetDeck();
    std::vector<std::uint8_t> currentHand(deckSpan.begin(), deckSpan.end());
    while (currentHand.size() < cardsNeeded && !m_cards.IsEmpty()) {
        currentHand.push_back(m_cards.DrawACard().GetValue());
    }
	player.SetDeck(currentHand);
}

void Game::StartGame()
{
    int cardsPerPlayer = (m_players.size() == 2) ? 8 : (m_players.size() == 3 ? 7 : 6);

    for (auto& player : m_players) {
        std::vector<std::uint8_t> hand;
        for (int i = 0; i < cardsPerPlayer && !m_cards.IsEmpty(); ++i) {
            hand.push_back(m_cards.DrawACard().GetValue());
        }
        player.SetDeck(hand);
    }
}

void Game::NextPlayer(int cardsPlayed)
{
    m_current_player_index = (m_current_player_index + 1) % m_players.size();
}

bool Game::CanPlayerMakeAtLeastOneMove(int playerIndex) const
{
    if (playerIndex >= m_players.size()) 
        return false;
    const auto& hand = m_players[playerIndex].GetDeck();

    for (std::uint8_t cardVal : hand) {
        for (int i = 0; i < 4; ++i) {
            if (m_play_piles_.IsMoveValid(static_cast<PlayPiles::StackIndex>(i), Card(cardVal)))
                return true;
        }
    }
    return false;
}

bool Game::PlaySingleCard(int userId, std::uint8_t cardValue, int stackIndex)
{
    Player& currentPlayer = m_players[m_current_player_index];
    if (currentPlayer.GetId() != userId) 
        return false;

    auto hand = currentPlayer.GetDeck();
    if (std::find(hand.begin(), hand.end(), cardValue) == hand.end()) 
        return false;

    auto stackIdx = static_cast<PlayPiles::StackIndex>(stackIndex);
    if (!m_play_piles_.IsMoveValid(stackIdx, Card(cardValue))) 
        return false;

    m_play_piles_.PlayCardOnStack(stackIdx, Card(cardValue));
    currentPlayer.RemoveCard(cardValue);
    m_cardsPlayedThisTurn++;
    return true;
}

bool Game::EndCurrentTurn(int userId) 
{
    if (m_players[m_current_player_index].GetId() != userId)
        return false;
    if (m_cardsPlayedThisTurn < GetMinCardsToPlay())
        return false;
    
	refillPlayerHand(m_players[m_current_player_index]);

    NextPlayer(m_cardsPlayedThisTurn);
    m_cardsPlayedThisTurn = 0;
    return true;
}

GameState Game::CheckGameState() const {
    bool handsEmpty = std::ranges::all_of(m_players, [](const Player& p) {
        return p.GetDeck().empty();
        });

    if (m_cards.IsEmpty() && handsEmpty)
        return GameState::Won;

    if (!CanPlayerMakeAtLeastOneMove(m_current_player_index)) {
        if (m_cardsPlayedThisTurn < GetMinCardsToPlay()) {
            return GameState::Lost;
        }
    }

    return GameState::InProgress;
}