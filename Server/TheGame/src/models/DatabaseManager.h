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
        ),

        make_table("matches",
            make_column("MatchId", &Match::MatchId, primary_key().autoincrement()),
            make_column("Status", &Match::Status),
            make_column("CurrentTurnPlayerId", &Match::CurrentTurnPlayerId),
            make_column("StacksStateJSON", &Match::StacksStateJSON),
            make_column("DeckStateJSON", &Match::DeckStateJSON)
        ),

        make_table("match_players",
            make_column("Id", &MatchPlayer::Id, primary_key().autoincrement()),
            make_column("MatchId", &MatchPlayer::MatchId),
            make_column("UserId", &MatchPlayer::UserId),
            make_column("CardsInHandJSON", &MatchPlayer::CardsInHandJSON),

            foreign_key(&MatchPlayer::MatchId).references(&Match::MatchId),
            foreign_key(&MatchPlayer::UserId).references(&UserProfile::Id)
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