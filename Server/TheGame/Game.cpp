#include "Game.h"
#include <algorithm>

Game::Game(std::vector<Player> players)
    : m_players(std::move(players)), m_cards(), m_play_piles_(), m_current_player_index(0)
{
    StartGame();
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
    Player& currentPlayer = m_players[m_current_player_index];

    std::vector<std::uint8_t> currentHand;
    auto deckSpan = currentPlayer.GetDeck();
    currentHand.assign(deckSpan.begin(), deckSpan.end());

    for (int i = 0; i < cardsPlayed && !m_cards.IsEmpty(); ++i) 
        currentHand.push_back(m_cards.DrawACard().GetValue());

    currentPlayer.SetDeck(currentHand);
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

    NextPlayer(m_cardsPlayedThisTurn);
    m_cardsPlayedThisTurn = 0;
    return true;
}

GameState Game::CheckGameState() const
{
    bool handsEmpty = std::all_of(m_players.begin(), m_players.end(),
        [](const Player& p) { return p.GetDeck().empty(); });

    if (m_cards.IsEmpty() && handsEmpty)
        return GameState::Won;
    return GameState::InProgress;
}