#include "BinaryGameService.h"

BinaryGameService::ActionResult BinaryGameService::ProcessPlayerAction(Game& game, int userId, const std::string& binaryData)
{
    myproject::GameMessage msg;
    if (!msg.ParseFromString(binaryData)) {
        return { false, "Failed to parse action message", false };
    }

    if (msg.type() != myproject::GameMessage::ACTION || !msg.has_action()) {
        return { false, "Invalid message type for action", false };
    }
    const myproject::PlayerAction& action = msg.action();

    if (action.action_type() == myproject::PlayerAction::PLAY_CARD) {
        if (game.PlaySingleCard(userId, action.card_value(), action.stack_index()))
        {
            return { true, "", false };
        }
        else
        {
            return { false, "Invalid move", false };
        }
    }
    else if (action.action_type() == myproject::PlayerAction_ActionType_END_TURN)
    {
        if (game.EndCurrentTurn(userId))
        {
            return { true, "", true };
        }
        else
        {
            return { false, "Cannot end turn now", false };
        }
    }
    return { false, "Unknown Action", false };
}

std::vector<std::pair<int, std::string>> BinaryGameService::PrepareBroadcastMessages(Game& game)
{
    std::vector<std::pair<int, std::string>> messages;
    const auto& players = game.GetPlayers();

    auto currentStatus = game.CheckGameState();

    for (const auto& targetPlayer : players) {
        myproject::GameMessage msgWrapper;

        if (currentStatus != GameState::InProgress) {
            msgWrapper.set_type(myproject::GameMessage_Type_GAME_OVER);
        }
        else {
            msgWrapper.set_type(myproject::GameMessage_Type_STATE_UPDATE);
        }

        msgWrapper.set_user_id(targetPlayer.GetId());

        myproject::GameState* stateProto = msgWrapper.mutable_state();

        stateProto->set_game_status(static_cast<myproject::GameState_Status>(currentStatus));

        stateProto->set_current_player_id(players[game.GetCurrentPlayerIndex()].GetId());

        stateProto->set_cards_remaining_in_deck(static_cast<int32_t>(game.GetDeckSize()));

        for (int i = 0; i < 4; ++i) {
            stateProto->add_stack_tops(game.GetPlayPiles().GetStackValue(static_cast<PlayPiles::StackIndex>(i)));
        }

        for (const auto& p : players) {
            auto* pInfo = stateProto->add_players();
            pInfo->set_user_id(p.GetId());
            pInfo->set_hand_size(static_cast<int32_t>(p.GetDeck().size()));

            if (p.GetId() == targetPlayer.GetId()) {
                for (uint8_t cardVal : p.GetDeck()) {
                    pInfo->add_your_cards(cardVal);
                }
            }
        }

        messages.push_back({ targetPlayer.GetId(), msgWrapper.SerializeAsString() });
    }
    return messages;
}