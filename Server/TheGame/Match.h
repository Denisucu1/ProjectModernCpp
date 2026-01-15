#pragma once

#include <string>
#include <cstdint>

auto initStorage();

namespace MatchConstants {
    inline const int MaxPlayersDefault = 5;
    inline const std::string StatusInitial = "Waiting for players";
    inline const std::string DifficultyDefault = "Normal";
    inline const std::string StacksInitial = "[1, 1, 100, 100]";
    inline const double ScoreInitial = 1.0;
}

class GameDB {
    friend auto initStorage();
    friend class MatchService;
public:
    int id = 0;
    int max_players = MatchConstants::MaxPlayersDefault;
    std::string status = MatchConstants::StatusInitial;
    std::string difficulty = MatchConstants::DifficultyDefault;
    std::string deck_state = "";
    std::string stacks_state = MatchConstants::StacksInitial;
    std::int64_t creation_timestamp = 0;
    double first_player_score = MatchConstants::ScoreInitial;

    GameDB() = default;
    int GetId() const;
    const std::string& GetStatus() const;
    const std::string& GetDifficulty() const;
    int GetMaxPlayers() const;
    const std::string& GetDeckState() const;
    const std::string& GetStacksState() const;
    std::int64_t GetCreationTimestamp() const;
    double GetFirstPlayerScore() const;
};

class PlayerDB {
    friend auto initStorage();
    friend class MatchService;
public:
    int id = 0;
    int user_id = 0;
    int game_id = 0;
    int seat_index = 0;
    std::string hand = "";

    PlayerDB() = default;
    int GetId() const;
    int GetUserId() const;
    int GetGameId() const;
    int Get_seat_index() const;
    const std::string& GetHand() const;
};

class Move {
    friend auto initStorage();
    friend class MatchService;
public:
    int id = 0;
    int player_id = 0;
    int game_id = 0;
    std::string cards_played = "";

    Move() = default;
    int GetId() const;
    int GetPlayerId() const;
    int GetGameId() const;
    const std::string& GetCardsPlayed() const;
};

class Chat {
    friend auto initStorage();
    friend class MatchService;
public:
    int id = 0;
    int player_id = 0;
    int game_id = 0;
    std::string message = "";

    Chat() = default;
    int GetId() const;
    int GetPlayerId() const;
    int GetGameId() const;
    const std::string& GetMessage() const;
};