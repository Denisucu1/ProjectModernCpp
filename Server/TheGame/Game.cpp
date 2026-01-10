#include "Game.h"
#include <algorithm>

Game::Game(std::vector<Player> players)
    : m_cards(),
    m_players(std::move(players)),
    m_play_piles_(),
    m_current_player_index(0),
    m_cardsPlayedThisTurn(0)
{
    StartGame();
}

void Game::StartGame() 
{
    int cardsPerPlayer = HAND_SIZE_DEFAULT;

    if (m_players.size() == TWO_PLAYERS_COUNT) 
        cardsPerPlayer = HAND_SIZE_TWO_PLAYERS;
    else 
    	if (m_players.size() == THREE_PLAYERS_COUNT) 
			cardsPerPlayer = HAND_SIZE_THREE_PLAYERS;


    for (auto& player : m_players) 
    {
        std::vector<std::uint8_t> hand;
        for (int i = 0; i < cardsPerPlayer && !m_cards.IsEmpty(); ++i) 
            hand.push_back(m_cards.DrawACard().GetValue());
        
        player.SetDeck(hand);
    }
}

const PlayPiles& Game::GetPlayPiles() const
{
	return m_play_piles_;
}

const DrawPile& Game::GetDrawPile() const
{
	return m_cards;
}

size_t Game::GetDeckSize() const
{
	return m_cards.GetSize();
}

const std::vector<Player>& Game::GetPlayers() const
{
	return m_players;
}

std::uint8_t Game::GetMinCardsToPlay() const 
{
    return m_cards.IsEmpty() ? MIN_CARDS_EMPTY_DECK : MIN_CARDS_FULL_DECK;
}

bool Game::PlaySingleCard(int userId, std::uint8_t cardValue, int stackIndex) 
{
    if (m_current_player_index >= m_players.size()) 
        return false;

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
    if (m_current_player_index >= m_players.size()) 
        return false;
    if (m_players[m_current_player_index].GetId() != userId) 
        return false;

    if (m_cardsPlayedThisTurn < GetMinCardsToPlay()) 
        return false;

    NextPlayer(m_cardsPlayedThisTurn);
    m_cardsPlayedThisTurn = 0;
    return true;
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
    m_current_player_index = static_cast<std::uint8_t>((m_current_player_index + 1) % m_players.size());
}

bool Game::CanPlayerMakeAtLeastOneMove(int playerIndex) const 
{
    if (playerIndex >= static_cast<int>(m_players.size())) 
        return false;

    const auto hand = m_players[playerIndex].GetDeck();
    for (std::uint8_t cardVal : hand) 
    {
        for (size_t i = 0; i < PlayPiles::STACK_COUNT; ++i) 
            if (m_play_piles_.IsMoveValid(static_cast<PlayPiles::StackIndex>(i), Card(cardVal)))
                return true;
        
    }
    return false;
}

GameState Game::CheckGameState() const 
{
    bool handsEmpty = std::all_of(m_players.begin(), m_players.end(),
        [](const Player& p) { return p.GetDeck().empty(); });

    if (m_cards.IsEmpty() && handsEmpty) 
        return GameState::Won;
    

    if (!CanPlayerMakeAtLeastOneMove(m_current_player_index)) 
        return GameState::Lost;
    

    return GameState::InProgress;
}