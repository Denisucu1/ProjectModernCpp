#pragma once

#include <string>
#include <cstdint>

namespace MatchConstants {
    inline const int MaxPlayersDefault = 5;
    inline const std::string StatusInitial = "Waiting for players";
    inline const std::string DifficultyDefault = "Normal";
    inline const std::string StacksInitial = "[1, 1, 100, 100]";
    inline const double ScoreInitial = 1.0;
}

class GameDB {
    friend class MatchService;
    friend class GameService;
private:
    int id = 0;
    int max_players = MatchConstants::MaxPlayersDefault;
    std::string status = MatchConstants::StatusInitial;
    std::string difficulty = MatchConstants::DifficultyDefault;
    std::string deck_state = "";
    std::string stacks_state = MatchConstants::StacksInitial;
    std::int64_t creation_timestamp = 0;
    double first_player_score = MatchConstants::ScoreInitial;

public:
    GameDB() = default;
    int GetId() const;
    void SetId(int v);
    const std::string& GetStatus() const;
    void SetStatus(std::string v);
    const std::string& GetDifficulty() const;
    void SetDifficulty(std::string v);
    int GetMaxPlayers() const;
    void SetMaxPlayers(int v);
    const std::string& GetDeckState() const;
    void SetDeckState(std::string v);
    const std::string& GetStacksState() const;
    void SetStacksState(std::string v);
    std::int64_t GetCreationTimestamp() const;
    void SetCreationTimestamp(std::int64_t v);
    double GetFirstPlayerScore() const;
    void SetFirstPlayerScore(double v);
};

class PlayerDB {
    friend class MatchService;
    friend class GameService;
private:
    int id = 0;
    int user_id = 0;
    int game_id = 0;
    int seat_index = 0;
    std::string hand = "";

public:
    PlayerDB() = default;
    int GetId() const;
    void SetId(int v);
    int GetUserId() const;
    void SetUserId(int v);
    int GetGameId() const;
    void SetGameId(int v);
    int GetSeatIndex() const;
    void SetSeatIndex(int v);
    const std::string& GetHand() const;
    void SetHand(std::string v);
};

class Move {
    friend class MatchService;
private:
    int id = 0;
    int player_id = 0;
    int game_id = 0;
    std::string cards_played = "";

public:
    Move() = default;
    int GetId() const;
    void SetId(int v);
    int GetPlayerId() const;
    void SetPlayerId(int v);
    int GetGameId() const;
    void SetGameId(int v);
    const std::string& GetCardsPlayed() const;
    void SetCardsPlayed(std::string v);
};

class Chat {
    friend class MatchService;
    friend class GameService;
private:
    int id = 0;
    int player_id = 0;
    int game_id = 0;
    std::string message = "";

public:
    Chat() = default;
    int GetId() const;
    void SetId(int v);
    int GetPlayerId() const;
    void SetPlayerId(int v);
    int GetGameId() const;
    void SetGameId(int v);
    const std::string& GetMessage() const;
    void SetMessage(std::string v);
};