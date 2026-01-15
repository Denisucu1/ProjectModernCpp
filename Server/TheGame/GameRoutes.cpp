#include "GameRoutes.h"
#include "Match.h"

namespace GameRoutes {
    void GameRoutes::setup(crow::SimpleApp& app, UserService& userSvc, GameService& gameSvc)
    {
        CROW_ROUTE(app, "/api/game_status/<int>").methods("GET"_method)
            ([&gameSvc](user_id userId) {
            crow::response res;
            res.add_header("Access-Control-Allow-Origin", "*");

            auto gameId = gameSvc.GetPlayerGameStatus(userId);

            if (gameId.has_value()) {
                res.code = 200;
                res.body = "{\"status\": \"in_game\", \"gameId\": \"" + gameId.value() + "\"}";
            }
            else {
                res.code = 200;
                res.body = "{\"status\": \"waiting\"}";
            }

            return res;
                });

        CROW_ROUTE(app, "/api/profile/<int>").methods("GET"_method)
            ([&userSvc](user_id userId) {
            crow::response res;
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Content-Type", "application/json");

            auto userProfile = userSvc.GetProfileById(userId);

            if (!userProfile) {
                res.code = 404;
                res.body = "{\"error\": \"User not found\"}";
                return res;
            }

            crow::json::wvalue profileJson;
            profileJson["userId"] = userProfile->GetUserId();
            profileJson["hoursPlayed"] = userProfile->GetHoursPlayed();
            profileJson["gamesPlayed"] = userProfile->GetGamesPlayed();
            profileJson["gamesWon"] = userProfile->GetGamesWon();
            profileJson["cardsLeftOnLosses"] = userProfile->GetCardsLeftOnLosses();
            profileJson["performanceScore"] = userProfile->GetPerformanceScore();
            res.code = 200;
            res.body = profileJson.dump();
            return res;
                });

        CROW_ROUTE(app, "/api/match/<int>").methods("GET"_method)
            ([&gameSvc](const crow::request& req, int matchId) {

            auto matchOpt = gameSvc.GetMatchState(matchId);

            if (!matchOpt.has_value()) {
                crow::json::wvalue errorRes;
                errorRes["error"] = "Match not found";
                return crow::response(404, errorRes);
            }

            const auto& match = matchOpt.value();
            crow::json::wvalue res;

            res["matchId"] = match.match_id;
            res["status"] = match.status;
            res["currentTurnPlayerId"] = match.current_turn_player_id;

            crow::json::wvalue stacksJson;
            int i = 0;
            for (const auto& val : match.stacks) {
                stacksJson[i++] = val;
            }
            res["stacksState"] = std::move(stacksJson);

            crow::json::wvalue deckObj;
            deckObj["cardsRemaining"] = match.deck_count;
            deckObj["topDiscard"] = 0;
            res["deckState"] = std::move(deckObj);

            std::vector<crow::json::wvalue> playersJsonList;

            for (const auto& player : match.players) {
                crow::json::wvalue pJson;
                pJson["id"] = player.id;
                pJson["userId"] = player.userId;

                crow::json::wvalue handJson;
                int k = 0;
                for (const auto& cardVal : player.cards_in_hand) {
                    handJson[k++] = cardVal;
                }
                pJson["cardsInHand"] = std::move(handJson);

                playersJsonList.push_back(std::move(pJson));
            }

            res["players"] = std::move(playersJsonList);

            return crow::response(200, res);
                });
    }
}