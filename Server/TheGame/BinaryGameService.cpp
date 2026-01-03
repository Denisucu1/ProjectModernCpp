#include "BinaryGameService.h"

BinaryGameService::ActionResult BinaryGameService::ProcessPlayerAction(Game& game, int userId, const std::string& binaryData)
{
	myproject::GameMessage msg;
	if(!msg.ParseFromString(binaryData)) {
		return { false, "Failed to parse action message", false };
	}

	if(msg.type() != myproject::GameMessage::ACTION || !msg.has_action()) {
		return { false, "Invalid message type for action", false };
	}
	const myproject::PlayerAction& action = msg.action();

	//if(action.action_type() != myproject::PlayerAction::PLAY_CARD) {
	//	if(game.PlaySingleCard(userId, action.card_id(), action.stack_index())) 
	//	{
	//		return { true, "", false };
	//	} 
	//	else 
	//	{
	//		return { false, "Invalid move", false };
	//	}
	//}
	//else if (action.action_type() == myproject::PlayerAction_ActionType_END_TURN)
	//{
	//	if (game.EndCurrentTurn(userId))
	//	{
	//		return { true, "", true };
	//	}
	//	else
	//	{
	//		return { false, "Cannot end turn now", false };
	//	}
	//}
	return { false, "Unknown Action", false };
}

std::vector<std::pair<int, std::string>> BinaryGameService::PrepareBroadcastMessages(Game& game)
{
	std::vector<std::pair<int, std::string>> messages;

	myproject::GameMessage msgWrapper;
	msgWrapper.set_type(myproject::GameMessage::STATE_UPDATE);
	myproject::GameState* stateProto = msgWrapper.mutable_state();

	const auto& players = game.GetPlayers();
	/*int currentIdx = game.GetCurrentPlayerIndex();
	if(currentIdx >= 0 && currentIdx < players.size())
		stateProto->set_user_id(players[currentIdx].GetId());*/

	//const auto& piles = game.GetPlayerPiles();
	//for (int i = 0; i < 4; i++)
	//{
	//	stateProto->add_stack_tops(piles.GetStackValue(static_cast<PlayPiles::StackIndex>(i)));
	//}

	for (const auto& p : players)
	{
		auto* pInfo = stateProto->add_players();
		pInfo->set_user_id(p.GetId());
		pInfo->set_hand_size(static_cast<int32_t>(p.GetDeck().size()));
	}

	for (const auto& p : players)
	{
		myproject::GameMessage personalizedMsg = msgWrapper;

		for (int i = 0; i < personalizedMsg.state().players_size(); ++i)
		{
			if (personalizedMsg.state().players(i).user_id() == p.GetId())
			{
				auto targetP = personalizedMsg.mutable_state()->mutable_players(i);
				for(auto cardVal : p.GetDeck())
				{
					targetP->add_your_cards(static_cast<int32_t>(cardVal));
				}
				break;
			}
		}
		messages.push_back({ p.GetId(), personalizedMsg.SerializeAsString() });
	}
	return messages;
}

