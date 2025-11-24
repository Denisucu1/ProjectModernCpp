#ifndef MATCH_SERVICE_H
#define MATCH_SERVICE_H

#include "../models/DatabaseManager.h"
#include "../models/Match.h" 
#include "../models/User.h" 
#include <vector>
#include <optional>
#include <string>

class MatchService {
public:

    std::optional<int> findOrCreateMatch(int requestingUserId);

    void startGame(int matchId, const std::vector<int>& userIds);
    void updateMatchState(int matchId, const std::string& newStacksState, const std::string& newDeckState, int nextPlayerId);

    // Poți adăuga metode pentru citirea stării jocului, de exemplu:
    // std::optional<Joc> getMatchState(int matchId); 
    // std::vector<Jucator> getPlayersInMatch(int matchId);
};

#endif // MATCH_SERVICE_H