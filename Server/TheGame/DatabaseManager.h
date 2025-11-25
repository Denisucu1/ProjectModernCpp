#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "User.h"
#include "Match.h" 

#include <sqlite_orm/sqlite_orm.h>
#include <string>

using namespace sqlite_orm;


inline auto initStorage() 
{
    const std::string db_path = "the_game_db.sqlite";

    return make_storage(db_path,
        make_table("Users",
            make_column("id", &User::id, primary_key().autoincrement()),
            make_column("username", &User::username,unique()),
            make_column("password", &User::password),
			make_column("SessionToken", &User::SessionToken),
			make_column("TokenExpiration", &User::TokenExpiration),
			make_column("LastActivity", &User::LastActivity)
            
        ),

        make_table("profiles",
            make_column("id", &Profile::id, primary_key().autoincrement()),
            make_column("user_id", &Profile::User_Id,unique()),
			make_column("hours_played", &Profile::Hours_Played),
			make_column("games_played", &Profile::Games_Played),
			make_column("games_won", &Profile::Games_Won),
            make_column("cards_left_on_losses", &Profile::Cards_left_on_losses),
            make_column("performance_score", &Profile::performance_score),
			foreign_key(&Profile::User_Id).references(&User::id)

            
        ),

        make_table("games",
            make_column("id", &Joc::id, primary_key().autoincrement()),
            make_column("status", &Joc::status),
            make_column("difficulty", &Joc::difficulty),
            make_column("max_players", &Joc::max_players),
            make_column("deck_state", &Joc::deck_state),
            make_column("stacks_state", &Joc::stacks_state)
        ),

        make_table("players",
            make_column("id", &Jucator::id, primary_key().autoincrement()),
            make_column("game_id", &Jucator::game_id),
            make_column("user_id", &Jucator::user_id),
            make_column("seat_index", &Jucator::seat_index),
            make_column("cards_played", &Jucator::hand),
            foreign_key(&Jucator::game_id).references(&Joc::id),
            foreign_key(&Jucator::user_id).references(&User::id)
        ),

        make_table("moves",
            make_column("id", &Move::id, primary_key().autoincrement()),
            make_column("player_id", &Move::player_id),
            make_column("game_id", &Move::game_id),
            make_column("cards_played", &Move::cards_played),
            foreign_key(&Move::player_id).references(&Jucator::id),
            foreign_key(&Move::game_id).references(&Joc::id)
        ),

        make_table("chats",
            make_column("id", &Chat::id, primary_key().autoincrement()),
            make_column("player_id", &Chat::player_id),
            make_column("game_id", &Chat::game_id),
            make_column("message", &Chat::message),
            foreign_key(&Chat::player_id).references(&Jucator::id),
            foreign_key(&Chat::game_id).references(&Joc::id)
        )
    );
}

using Storage = decltype(initStorage());

inline Storage& getStorage() 
{
    static Storage storage = initStorage();
    static bool schema_synced = (storage.sync_schema(), true);
    return storage;
}

#endif // DATABASE_MANAGER_H