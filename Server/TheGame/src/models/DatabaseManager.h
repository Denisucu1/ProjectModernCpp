#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "User.h"
#include <sqlite_orm/sqlite_orm.h> 
#include <string>

using namespace sqlite_orm;

inline auto initStorage() 
{
    const std::string db_path = "the_game_db.sqlite";

    return make_storage(db_path,
        make_table("users",

            make_column("username", &User::username, unique()),
            make_column("password", &User::password),
            make_column("id", &User::id, primary_key().autoincrement()),
            make_column("games_played", &User::games_played),
            make_column("games_won", &User::games_won),
            make_column("total_cards_at_loss", &User::total_cards_at_loss),
            make_column("total_time_minutes", &User::total_time_minutes),
            make_column("performance_score", &User::performance_score)
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
