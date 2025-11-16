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
            make_column("Username", &UserProfile::Username, unique()),
            make_column("Password", &UserProfile::Password),
            make_column("Id", &UserProfile::Id, primary_key().autoincrement()),
            make_column("GamesPlayed", &UserProfile::GamesPlayed),
            make_column("GamesWon", &UserProfile::GamesWon),
            make_column("TotalCardsAtLoss", &UserProfile::TotalCardsAtLoss),
            make_column("TotalTimeMinutes", &UserProfile::TotalTimeMinutes),
            make_column("PerformanceScore", &UserProfile::PerformanceScore),
            make_column("SessionToken", &UserProfile::SessionToken),
            make_column("TokenExpiration", &UserProfile::TokenExpiration),
            make_column("LastActivity", &UserProfile::LastActivity)
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