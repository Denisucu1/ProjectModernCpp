#pragma once

#include "DatabaseManager.h"
#include "Match.h" 
#include "User.h"  
#include <vector>
#include <optional>
#include <string>

class MatchService {
public:
    std::optional<int> FindOrCreateMatch(int requestingUserId);
    void StartGame(int matchId, const std::vector<int>& userIds);
    void UpdateMatchState(int matchId, const std::string& newStacksState, const std::string& newDeckState, int nextPlayerId);
    void CheckForTimeouts();
};