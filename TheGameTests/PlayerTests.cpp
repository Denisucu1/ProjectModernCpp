#include "gtest/gtest.h"
#include "Player.h" 

TEST(PlayerTest, CardRemovalSuccessful) {
    Player p("Alice", 1);
    p.SetDeck({ 10, 20, 30 });

    ASSERT_TRUE(p.RemoveCard(20));
    ASSERT_EQ(2, p.GetDeck().size());

    bool found = false;
    for (auto card : p.GetDeck()) {
        if (card == 20) found = true;
    }
    ASSERT_FALSE(found);
}

TEST(PlayerTest, CardRemovalFailed) {
    Player p("Bob", 2);
    p.SetDeck({ 5, 15 });

    ASSERT_FALSE(p.RemoveCard(10));
    ASSERT_EQ(2, p.GetDeck().size());
}

TEST(PlayerTest, ScoreManagement) {
    Player p("Charlie", 3);
    p.SetScore(15);
    ASSERT_EQ(15, p.GetScore());
}

TEST(PlayerTest, DeckIsSetCorrectly) {
    Player p("Charlie", 3);
    std::vector<std::uint8_t> newDeck = { 1, 5, 9 };
    p.SetDeck(newDeck);

    ASSERT_EQ(newDeck, p.GetDeck());
}