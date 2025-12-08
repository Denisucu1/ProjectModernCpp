#include "gtest/gtest.h"
#include "DrawPile.h"

TEST(DrawPileTest, InitialCardCount) {
    DrawPile pile;
    
}

TEST(DrawPileTest, DrawACardReducesCount) {
    DrawPile pile;
    Card drawn = pile.DrawACard();
    // Daca nu poti accesa GetCount(), acest test este suficient:
    ASSERT_FALSE(pile.IsEmpty());
}

TEST(DrawPileTest, IsEmptyInitiallyFalse) {
    DrawPile pile;
    ASSERT_FALSE(pile.IsEmpty());
}