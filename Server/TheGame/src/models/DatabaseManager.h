#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "User.h"
#include <sqlite_orm/sqlite_orm.h>
#include <string>

using namespace sqlite_orm;

inline auto initStorage() {
    const std::string db_path = "the_game_db.sqlite";

    return make_storage(db_path,
        make_table("users",
            make_column("Username", &User::Username, unique()),
            make_column("Password", &User::Password),
            make_column("Id", &User::Id, primary_key().autoincrement()),
            make_column("GamesPlayed", &User::GamesPlayed),
            make_column("GamesWon", &User::GamesWon),
            make_column("TotalCardsAtLoss", &User::TotalCardsAtLoss),
            make_column("TotalTimeMinutes", &User::TotalTimeMinutes),
            make_column("PerformanceScore", &User::PerformanceScore)
        )
    );
}

using Storage = decltype(initStorage());

inline Storage& getStorage() {
    static Storage storage = initStorage();
    static bool schema_synced = (storage.sync_schema(), true);
    return storage;
}

#endif // DATABASE_MANAGER_H