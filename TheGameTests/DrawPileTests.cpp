#include "gtest/gtest.h"
#include "DrawPile.h"

TEST(DrawPileTest, InitialCardCount) {
    DrawPile pile;
    
}

TEST(DrawPileTest, DrawACardReducesCount) {
    DrawPile pile;
    Card drawn = pile.DrawACard();
    ASSERT_FALSE(pile.IsEmpty());
}

TEST(DrawPileTest, IsEmptyInitiallyFalse) {
    DrawPile pile;
    ASSERT_FALSE(pile.IsEmpty());
}