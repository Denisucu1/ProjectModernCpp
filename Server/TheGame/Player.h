#pragma once
#include <array>
#include <string>
#include <cstdint>
#include <vector>

class Player
{
public:
    Player(std::string name, int id);

    Player(Player&& other);
    Player& operator=(Player&& other);

    Player(const Player& other) = delete;
    Player& operator=(const Player& other) = delete;

    void SetDeck(const std::vector<std::uint8_t>& deck);

    void SetScore(std::uint8_t score);

    std::vector<std::uint8_t> GetDeck() const;

    std::uint8_t GetScore() const;

    bool RemoveCard(std::uint8_t cardValue);

    int GetId() const;

private:
    std::string m_name;
    std::array<std::uint8_t, 8> m_deck;
    std::uint8_t m_deck_active_size; 
    std::uint8_t m_score;
    int m_id;
};