#pragma once
#include <sqlite_orm/sqlite_orm.h>
#include "User.h"
#include "Match.h"

using namespace sqlite_orm;

inline auto initStorage() {
    const std::string db_path = "the_game_db.sqlite";

    return make_storage(db_path,
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
            foreign_key(&Profile::SetUserId).references(&User::GetId)
        ),
        make_table("games",
            make_column("id", &GameEntity::SetId, &GameEntity::GetId, primary_key().autoincrement()),
            make_column("status", &GameEntity::SetStatus, &GameEntity::GetStatus),
            make_column("difficulty", &GameEntity::SetDifficulty, &GameEntity::GetDifficulty),
            make_column("max_players", &GameEntity::SetMaxPlayers, &GameEntity::GetMaxPlayers),
            make_column("deck_state", &GameEntity::SetDeckState, &GameEntity::GetDeckState),
            make_column("stacks_state", &GameEntity::SetStacksState, &GameEntity::GetStacksState),
            make_column("first_player_score", &GameEntity::SetFirstPlayerScore, &GameEntity::GetFirstPlayerScore),
            make_column("creation_timestamp", &GameEntity::SetCreationTimestamp, &GameEntity::GetCreationTimestamp)
        ),
        make_table("players",
            make_column("id", &PlayerParticipant::SetId, &PlayerParticipant::GetId, primary_key().autoincrement()),
            make_column("game_id", &PlayerParticipant::SetGameId, &PlayerParticipant::GetGameId),
            make_column("user_id", &PlayerParticipant::SetUserId, &PlayerParticipant::GetUserId),
            make_column("seat_index", &PlayerParticipant::SetSeatIndex, &PlayerParticipant::GetSeatIndex),
            make_column("hand_state", &PlayerParticipant::SetHand, &PlayerParticipant::GetHand),
            foreign_key(&PlayerParticipant::SetGameId).references(&GameEntity::GetId),
            foreign_key(&PlayerParticipant::SetUserId).references(&User::GetId)
        ),
        make_table("moves",
            make_column("id", &MoveEntity::SetId, &MoveEntity::GetId, primary_key().autoincrement()),
            make_column("player_id", &MoveEntity::SetPlayerId, &MoveEntity::GetPlayerId),
            make_column("game_id", &MoveEntity::SetGameId, &MoveEntity::GetGameId),
            make_column("cards_played", &MoveEntity::SetCardsPlayed, &MoveEntity::GetCardsPlayed),
            foreign_key(&MoveEntity::SetPlayerId).references(&PlayerParticipant::GetId),
            foreign_key(&MoveEntity::SetGameId).references(&GameEntity::GetId)
        ),
        make_table("chats",
            make_column("id", &ChatEntity::SetId, &ChatEntity::GetId, primary_key().autoincrement()),
            make_column("player_id", &ChatEntity::SetPlayerId, &ChatEntity::GetPlayerId),
            make_column("game_id", &ChatEntity::SetGameId, &ChatEntity::GetGameId),
            make_column("message", &ChatEntity::SetText, &ChatEntity::GetText),
            foreign_key(&ChatEntity::SetPlayerId).references(&PlayerParticipant::GetId),
            foreign_key(&ChatEntity::SetGameId).references(&GameEntity::GetId)
        ),
        make_index("idx_match_status_score", &GameEntity::GetStatus, &GameEntity::GetFirstPlayerScore)
    );
}

using Storage = decltype(initStorage());

inline Storage& getStorage() {
    static Storage storage = initStorage();
    static bool schema_synced = (storage.sync_schema(), true);
    return storage;
}