#pragma once

#include "User.h"
#include "Match.h"
#include <sqlite_orm/sqlite_orm.h>
#include <string>

using namespace sqlite_orm;

namespace DatabaseConfig {
    inline const std::string Path = "the_game_db.sqlite";
}

inline auto initStorage()
{
    return make_storage(DatabaseConfig::Path,
        make_table("Users",
            make_column("id", &User::id, primary_key().autoincrement()),
            make_column("username", &User::username, unique()),
            make_column("password", &User::password),
            make_column("SessionToken", &User::SessionToken),
            make_column("TokenExpiration", &User::TokenExpiration),
            make_column("LastActivity", &User::LastActivity)
        ),
        make_table("profiles",
            make_column("id", &Profile::id, primary_key().autoincrement()),
            make_column("user_id", &Profile::user_id, unique()),
            make_column("hours_played", &Profile::hours_played),
            make_column("games_played", &Profile::games_played),
            make_column("games_won", &Profile::games_won),
            make_column("cards_left_on_losses", &Profile::cards_left_on_losses),
            make_column("performance_score", &Profile::performance_score),
            foreign_key(&Profile::user_id).references(&User::id)
        ),
        make_table("games",
            make_column("id", &GameDB::id, primary_key().autoincrement()),
            make_column("status", &GameDB::status),
            make_column("difficulty", &GameDB::difficulty),
            make_column("max_players", &GameDB::max_players),
            make_column("deck_state", &GameDB::deck_state),
            make_column("stacks_state", &GameDB::stacks_state),
            make_column("first_player_score", &GameDB::first_player_score),
            make_column("creation_timestamp", &GameDB::creation_timestamp)
        ),
        make_table("players",
            make_column("id", &PlayerDB::id, primary_key().autoincrement()),
            make_column("game_id", &PlayerDB::game_id),
            make_column("user_id", &PlayerDB::user_id),
            make_column("seat_index", &PlayerDB::seat_index),
            make_column("hand_state", &PlayerDB::hand),
            foreign_key(&PlayerDB::game_id).references(&GameDB::id),
            foreign_key(&PlayerDB::user_id).references(&User::id)
        ),
        make_table("moves",
            make_column("id", &Move::id, primary_key().autoincrement()),
            make_column("player_id", &Move::player_id),
            make_column("game_id", &Move::game_id),
            make_column("cards_played", &Move::cards_played),
            foreign_key(&Move::player_id).references(&PlayerDB::id),
            foreign_key(&Move::game_id).references(&GameDB::id)
        ),
        make_table("chats",
            make_column("id", &Chat::id, primary_key().autoincrement()),
            make_column("player_id", &Chat::player_id),
            make_column("game_id", &Chat::game_id),
            make_column("message", &Chat::message),
            foreign_key(&Chat::player_id).references(&PlayerDB::id),
            foreign_key(&Chat::game_id).references(&GameDB::id)
        ),
        make_index("idx_match_status_score", &GameDB::status, &GameDB::first_player_score)
    );
}

using Storage = decltype(initStorage());
Storage& getStorage();