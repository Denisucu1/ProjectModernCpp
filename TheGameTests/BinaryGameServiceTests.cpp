#include "gtest/gtest.h"
#include "BinaryGameService.h"
#include "Game.h"
#include "Player.h"
#include "GameProtocol.pb.h" //

std::vector<Player> create_test_players() {
    std::vector<Player> players;
    players.emplace_back("Andrei", 101);
    players.emplace_back("Matei", 102);
    return players;
}

TEST(BinaryGameServiceTest, ShouldProcessValidPlayCardAction) {
    Game game(create_test_players());
    int currentUserId = game.GetPlayers()[game.GetCurrentPlayerIndex()].GetId();

    uint8_t cardValue = game.GetPlayers()[game.GetCurrentPlayerIndex()].GetDeck()[0];

    myproject::GameMessage msg;
    msg.set_type(myproject::GameMessage::ACTION);
    auto* action = msg.mutable_action();
    action->set_action_type(myproject::PlayerAction::PLAY_CARD);
    action->set_card_value(cardValue);
    action->set_stack_index(0); // Ascend 1

    std::string binaryData = msg.SerializeAsString();

    auto result = BinaryGameService::ProcessPlayerAction(game, currentUserId, binaryData);

    ASSERT_TRUE(result.success) << "Mutarea ar trebui sa fie valida conform logicii binare.";
}

TEST(BinaryGameServiceTest, ShouldReturnErrorForInvalidCard) {
    Game game(create_test_players());
    int currentUserId = game.GetPlayers()[game.GetCurrentPlayerIndex()].GetId();

    myproject::GameMessage msg;
    msg.set_type(myproject::GameMessage::ACTION);
    auto* action = msg.mutable_action();
    action->set_action_type(myproject::PlayerAction::PLAY_CARD);
    action->set_card_value(1); // Cartea 1 nu există în pachet (pachetul are 2-99)
    action->set_stack_index(0);

    std::string binaryData = msg.SerializeAsString();
    auto result = BinaryGameService::ProcessPlayerAction(game, currentUserId, binaryData);

    ASSERT_FALSE(result.success);
    ASSERT_EQ("Invalid move", result.message);
}

TEST(BinaryGameServiceTest, ShouldPrepareCorrectBroadcastMessages) {
    Game game(create_test_players());

    auto messages = BinaryGameService::PrepareBroadcastMessages(game);

    ASSERT_EQ(2, messages.size()); 

    myproject::GameMessage response;
    response.ParseFromString(messages[0].second);

    ASSERT_EQ(myproject::GameMessage::STATE_UPDATE, response.type());
    ASSERT_EQ(98 - 16, response.state().cards_remaining_in_deck()); 
}