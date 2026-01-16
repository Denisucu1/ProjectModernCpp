#pragma once
#include <string>
#include <cstdint>

class GameEntity {

private:

    int m_id = 0;
    int m_maxPlayers = 5;
    std::string m_status = "Waiting for players";
    std::string m_difficulty = "Normal";
    std::string m_deckState = "";
    std::string m_stacksState = "[1, 1, 100, 100]";
    std::int64_t m_creationTimestamp = 0;
    double m_firstPlayerScore = 1.0;

public:
    GameEntity();

    int GetId() const;
    int GetMaxPlayers() const;
    const std::string& GetStatus() const;
    const std::string& GetDifficulty() const;
    const std::string& GetDeckState() const;
    const std::string& GetStacksState() const;
    std::int64_t GetCreationTimestamp() const;
    double GetFirstPlayerScore() const;

    void SetId(int id);
    void SetMaxPlayers(int count);
    void SetStatus(const std::string& status);
    void SetDifficulty(const std::string& diff);
    void SetDeckState(const std::string& state);
    void SetStacksState(const std::string& state);
    void SetCreationTimestamp(std::int64_t ts);
    void SetFirstPlayerScore(double score);
};

class PlayerParticipant {

private:

    int m_id = 0;
    int m_userId = 0;
    int m_gameId = 0;
    int m_seatIndex = 0;
    std::string m_hand = "";

public:
    PlayerParticipant();

    int GetId() const;
    int GetUserId() const;
    int GetGameId() const;
    int GetSeatIndex() const;
    const std::string& GetHand() const;

    void SetId(int id);
    void SetUserId(int id);
    void SetGameId(int id);
    void SetSeatIndex(int index);
    void SetHand(const std::string& hand);
};

class MoveEntity {

private:

    int m_id = 0;
    int m_playerId = 0;
    int m_gameId = 0;
    std::string m_cardsPlayed = "";
public:
    MoveEntity();

    int GetId() const;
    int GetPlayerId() const;
    int GetGameId() const;
    const std::string& GetCardsPlayed() const;

    void SetId(int id);
    void SetPlayerId(int id);
    void SetGameId(int id);
    void SetCardsPlayed(const std::string& cards);
};

class ChatEntity {

private:

    int m_id = 0;
    int m_playerId = 0;
    int m_gameId = 0;
    std::string m_message = "";

public:
    ChatEntity();

    int GetId() const;
    int GetPlayerId() const;
    int GetGameId() const;
    const std::string& GetText() const;

    void SetId(int id);
    void SetPlayerId(int id);
    void SetGameId(int id);
    void SetText(const std::string& msg);
};