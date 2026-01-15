#pragma once

#include "User.h"
#include "Match.h"
#pragma warning(push)
#pragma warning(disable: 26495)
#include <sqlite_orm/sqlite_orm.h>
#pragma warning(pop)
#include <string>

using namespace sqlite_orm;

namespace DatabaseConfig {
    inline const std::string Path = "the_game_db.sqlite";
}

inline auto initStorage()
{
    return make_storage(DatabaseConfig::Path,
        make_table("Users",
            make_column("id", &User::SetId, &User::GetId, primary_key().autoincrement()),
            make_column("username", &User::SetUsername, &User::GetUsername, unique()),
            make_column("password", &User::SetPassword, &User::GetPassword),
            make_column("SessionToken", &User::SetSessionToken, &User::GetSessionToken),
            make_column("TokenExpiration", &User::SetTokenExpiration, &User::GetTokenExpiration),
            make_column("LastActivity", &User::SetLastActivity, &User::GetLastActivity)
        ),
        make_table("profiles",
            make_column("id", &Profile::SetId, &Profile::GetId, primary_key().autoincrement()),
            make_column("user_id", &Profile::SetUserId, &Profile::GetUserId, unique()),
            make_column("hours_played", &Profile::SetHoursPlayed, &Profile::GetHoursPlayed),
            make_column("games_played", &Profile::SetGamesPlayed, &Profile::GetGamesPlayed),
            make_column("games_won", &Profile::SetGamesWon, &Profile::GetGamesWon),
            make_column("cards_left_on_losses", &Profile::SetCardsLeftOnLosses, &Profile::GetCardsLeftOnLosses),
            make_column("performance_score", &Profile::SetPerformanceScore, &Profile::GetPerformanceScore),
            foreign_key(&Profile::SetUserId, &Profile::GetUserId).references(&User::SetId, &User::GetId)
        ),
        make_table("games",
            make_column("id", &GameDB::SetId, &GameDB::GetId, primary_key().autoincrement()),
            make_column("max_players", &GameDB::SetMaxPlayers, &GameDB::GetMaxPlayers),
            make_column("status", &GameDB::SetStatus, &GameDB::GetStatus),
            make_column("difficulty", &GameDB::SetDifficulty, &GameDB::GetDifficulty),
            make_column("deck_state", &GameDB::SetDeckState, &GameDB::GetDeckState),
            make_column("stacks_state", &GameDB::SetStacksState, &GameDB::GetStacksState),
            make_column("creation_timestamp", &GameDB::SetCreationTimestamp, &GameDB::GetCreationTimestamp),
            make_column("first_player_score", &GameDB::SetFirstPlayerScore, &GameDB::GetFirstPlayerScore)
        ),
        make_table("players",
            make_column("id", &PlayerDB::SetId, &PlayerDB::GetId, primary_key().autoincrement()),
            make_column("game_id", &PlayerDB::SetGameId, &PlayerDB::GetGameId),
            make_column("user_id", &PlayerDB::SetUserId, &PlayerDB::GetUserId),
            make_column("seat_index", &PlayerDB::SetSeatIndex, &PlayerDB::GetSeatIndex),
            make_column("hand_state", &PlayerDB::SetHand, &PlayerDB::GetHand),
            foreign_key(&PlayerDB::SetGameId, &PlayerDB::GetGameId).references(&GameDB::SetId, &GameDB::GetId),
            foreign_key(&PlayerDB::SetUserId, &PlayerDB::GetUserId).references(&User::SetId, &User::GetId)
        ),
        make_table("moves",
            make_column("id", &Move::SetId, &Move::GetId, primary_key().autoincrement()),
            make_column("player_id", &Move::SetPlayerId, &Move::GetPlayerId),
            make_column("game_id", &Move::SetGameId, &Move::GetGameId),
            make_column("cards_played", &Move::SetCardsPlayed, &Move::GetCardsPlayed),
            foreign_key(&Move::SetPlayerId, &Move::GetPlayerId).references(&PlayerDB::SetId, &PlayerDB::GetId),
            foreign_key(&Move::SetGameId, &Move::GetGameId).references(&GameDB::SetId, &GameDB::GetId)
        ),
        make_table("chats",
            make_column("id", &Chat::SetId, &Chat::GetId, primary_key().autoincrement()),
            make_column("player_id", &Chat::SetPlayerId, &Chat::GetPlayerId),
            make_column("game_id", &Chat::SetGameId, &Chat::GetGameId),
            make_column("message", &Chat::SetMessage, &Chat::GetMessage),
            foreign_key(&Chat::SetPlayerId, &Chat::GetPlayerId).references(&PlayerDB::SetId, &PlayerDB::GetId),
            foreign_key(&Chat::SetGameId, &Chat::GetGameId).references(&GameDB::SetId, &GameDB::GetId)
        )
    );
}