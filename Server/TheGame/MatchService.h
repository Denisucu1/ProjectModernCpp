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

    std::optional<int> FindOrCreateMatch(int requestingUserId);

    void StartGame(int matchId, const std::vector<int>& userIds);
    void UpdateMatchState(int matchId, const std::string& newStacksState, const std::string& newDeckState, int nextPlayerId);
    void CheckForTimeouts();
};

#endif // MATCH_SERVICE_H