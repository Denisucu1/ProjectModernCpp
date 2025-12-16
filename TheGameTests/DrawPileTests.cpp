#include "gtest/gtest.h"
#include "DrawPile.h"

TEST(DrawPileTest, InitialCardCount) 
{
    DrawPile pile;
    ASSERT_EQ(98, pile.GetSize());
}

TEST(DrawPileTest, DrawACardReducesCount) 
{
    DrawPile pile;
    Card drawn = pile.DrawACard();
    ASSERT_EQ(97, pile.GetSize());
    ASSERT_FALSE(pile.IsEmpty());
}

TEST(DrawPileTest, IsEmptyInitiallyFalse) 
{
    DrawPile pile;
    ASSERT_FALSE(pile.IsEmpty());
}