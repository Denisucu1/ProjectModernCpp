#include "gtest/gtest.h"
#include "UserService.h"

TEST(UserServiceLogic, FormulaTest) 
{
    EXPECT_EQ(UserService::ComputeScoreLogic(10, 10, 0), 5);
    EXPECT_EQ(UserService::ComputeScoreLogic(0, 10, 80), 1);
}

TEST(UserServiceLogic, ZeroGamesPlayedScore)
{
    int score = UserService::ComputeScoreLogic(0, 0, 0);
    EXPECT_EQ(score, 1);
}