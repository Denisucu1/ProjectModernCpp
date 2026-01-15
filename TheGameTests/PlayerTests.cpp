#include "gtest/gtest.h"
#include "Player.h"

TEST(PlayerTest, InitializationAndIdentity) {
    Player p("Andrei", 123);
    ASSERT_EQ(123, p.GetId());
}

TEST(PlayerTest, SetAndGetDeck) {
    Player p("Test", 1);
    std::vector<uint8_t> hand = { 10, 20, 30 };
    p.SetDeck(hand);

    auto deckView = p.GetDeckView();
    ASSERT_EQ(3, deckView.size());
    ASSERT_EQ(10, deckView[0]);
}

TEST(PlayerTest, RemoveCardLogic) {
    Player p("Test", 1);
    p.SetDeck({ 10, 20, 30 });

    ASSERT_TRUE(p.RemoveCard(20));
    ASSERT_EQ(2, p.GetDeckView().size());
    for (auto c : p.GetDeckView()) ASSERT_NE(20, c);
}

TEST(PlayerTest, MoveSemantics) {
    Player p1("Original", 1);
    p1.SetDeck({ 5, 10 });

    Player p2 = std::move(p1);
    ASSERT_EQ(2, p2.GetDeckView().size());
    ASSERT_EQ(0, p1.GetDeckView().size()); 
}