
#ifndef MATCH_H
#define MATCH_H

#include <string>

struct Match {
    int MatchId = 0; 
    std::string Status = "WAITING_FOR_PLAYERS"; 
    int CurrentTurnPlayerId = 0; 

    std::string StacksStateJSON = "[1, 1, 100, 100]";

    std::string DeckStateJSON = "";
};

struct MatchPlayer {
    int Id = 0; 
    int MatchId = 0; 
    int UserId = 0; 

    std::string CardsInHandJSON = "";
};

#endif // MATCH_H
