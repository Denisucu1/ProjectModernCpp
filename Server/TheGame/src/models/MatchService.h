#ifndef MATCH_SERVICE_H
#define MATCH_SERVICE_H

#include "../models/DatabaseManager.h"
#include "../models/Match.h"
#include "../models/User.h"
#include <vector>
#include <optional>

class MatchService {
public:

    std::optional<int> findOrCreateMatch(int requestingUserId);


    void startGame(int matchId, const std::vector<int>& userIds);
    void updateMatchState(int matchId, const std::string& newState);

};

#endif // MATCH_SERVICE_H
