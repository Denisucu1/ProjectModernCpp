
#ifndef MATCH_H
#define MATCH_H

#include <string>


class Joc {

public:

    int id = 0;
    std::string status = "Waiting for players";
    std::string difficulty = "Normal";
    int max_players = 5;
    std::string deck_state = "";
    std::string stacks_state = "[1, 1, 100, 100]";
    std::int64_t creation_timestamp = 0;

public:
    Joc() = default;

    int get_id() const 
    { 
        return id; 
    }
    const std::string& get_status() const 
    { 
        return status; 
    }
    const std::string& get_difficulty() const 
    { 
        return difficulty; 
    }
    int get_max_players() const 
    { 
        return max_players; 
    }
    const std::string& get_deck_state() const 
    { 
        return deck_state; 
    }
    const std::string& get_stacks_state() const 
    { 
        return stacks_state; 
    }

    std::int64_t get_creation_timestamp() const
    {
        return creation_timestamp;
    }

};

class Jucator {

public:

    int id = 0;
    int user_id = 0;
    int game_id = 0;
    int seat_index = 0;
    std::string hand = "";
    std::string cards_played = "";

public:
    Jucator() = default;

    int get_id() const 
    { 
        return id; 
    }
    int get_user_id() const 
    { 
        return user_id; 
    }
    int get_game_id() const 
    { 
        return game_id; 
    }
    int get_seat_index() const 
    { 
        return seat_index; 
    }
    const std::string& get_hand() const 
    { 
        return hand; 
    }
    const std::string& get_cards_played() const 
    { 
        return cards_played; 
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

    int get_id() const 
    { 
        return id; 
    }
    int get_player_id() const 
    { 
        return player_id; 
    }
    int get_game_id() const 
    { 
        return game_id; 
    }
    const std::string& get_cards_played() const 
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

    int get_id() const 
    { 
        return id; 
    }
    int get_player_id() const 
    { 
        return player_id; 
    }
    int get_game_id() const 
    { 
        return game_id; 
    }
    const std::string& get_message() const 
    { 
        return message; 
    }
};


#endif // MATCH_H
