#include "gtest/gtest.h"
#include "Card.h"

TEST(CardTest, CorrectInitialization) {
    Card c(42);
    ASSERT_EQ(42, c.GetValue());
}

TEST(CardTest, EdgeValueCheck) {
    Card min(2);
    Card max(99);
    ASSERT_EQ(2, min.GetValue());
    ASSERT_EQ(99, max.GetValue());
}