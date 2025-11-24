
#ifndef MATCH_H
#define MATCH_H

#include <string>


struct Joc {
    
    int id = 0;

	std::string status = "Waiting for players";
	std::string difficulty = "Normal";
	int max_players = 5;

	std::string deck_state = "";
	std::string stacks_state = "[1, 1, 100, 100]";

};

struct Jucator {

    int id = 0; 
    int player_id = 0; // foreign key -> players
    int game_id = 0; // foreign key -> games
    int seat_index = 0;
    std::string cards_played = "";
};

struct Move {
    int id = 0; 
    int player_id = 0; 
    int game_id = 0; 

    std::string cards_played = "";
};

struct Chat {

    int id = 0; 
	int player_id = 0; // foreign key -> players
    int game_id = 0; // foreign key -> games
    std::string message = "";
};


#endif // MATCH_H
