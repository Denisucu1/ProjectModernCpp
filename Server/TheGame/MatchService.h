#ifndef MATCH_SERVICE_H
#define MATCH_SERVICE_H

#include "DatabaseManager.h"
#include "Match.h" 
#include "User.h"  
#include <vector>
#include <optional>
#include <string>
#include <tuple>

class MatchService {
public:

    std::optional<int> findOrCreateMatch(int requestingUserId);

    void startGame(int matchId, const std::vector<int>& userIds);
    void updateMatchState(int matchId, const std::string& newStacksState, const std::string& newDeckState, int nextPlayerId);

};

#endif // MATCH_SERVICE_H