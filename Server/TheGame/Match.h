
#ifndef MATCH_H
#define MATCH_H

#include <string>


class Joc {

public:

    int id = 0;
    int max_players = 5;

    std::string status = "Waiting for players";
    std::string difficulty = "Normal";
    std::string deck_state = "";
    std::string stacks_state = "[1, 1, 100, 100]";
    std::int64_t creation_timestamp = 0;

    double first_player_score = 1.0;

public:
    Joc() = default;

    int GetId() const 
    { 
        return id; 
    }
    const std::string& GetStatus() const 
    { 
        return status; 
    }
    const std::string& GetDifficulty() const 
    { 
        return difficulty; 
    }
    int GetMaxPlayers() const 
    { 
        return max_players; 
    }
    const std::string& GetDeckState() const 
    { 
        return deck_state; 
    }
    const std::string& GetStacksState() const 
    { 
        return stacks_state; 
    }

    std::int64_t GetCreationTimestamp() const
    {
        return creation_timestamp;
    }

    double GetFirstPlayerScore() const
    {
        return first_player_score;
    }

};

class Jucator {

public:

    int id = 0;
    int user_id = 0;
    int game_id = 0;
    int seat_index = 0;
    std::string hand = "";

public:
    Jucator() = default;

    int GetId() const 
    { 
        return id; 
    }
    int GetUserId() const 
    { 
        return user_id; 
    }
    int GetGameId() const 
    { 
        return game_id; 
    }
    int GetSeatIndex() const 
    { 
        return seat_index; 
    }
    const std::string& GetHand() const 
    { 
        return hand; 
    }
    
};

class Move {

public:

    int id = 0;
    int player_id = 0;
    int game_id = 0;
    std::string cards_played = "";

public:
    Move() = default;

    int GetId() const 
    { 
        return id; 
    }
    int GetPlayerId() const 
    { 
        return player_id; 
    }
    int GetGameId() const 
    { 
        return game_id; 
    }
    const std::string& GetCardsPlayed() const 
    { 
        return cards_played; 
    }
};

class Chat {

public:

    int id = 0;
    int player_id = 0;
    int game_id = 0;
    std::string message = "";

public:
    Chat() = default;

    int GetId() const 
    { 
        return id; 
    }
    int GetPlayerId() const 
    { 
        return player_id; 
    }
    int GetGameId() const 
    { 
        return game_id; 
    }
    const std::string& GetMessage() const 
    { 
        return message; 
    }
};


#endif // MATCH_H
