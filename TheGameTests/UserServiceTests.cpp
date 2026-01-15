#include "gtest/gtest.h"
#include "UserService.h"

TEST(UserServiceLogic, FormulaTest) 
{
    EXPECT_EQ(UserService::ComputeScoreLogic(10, 10, 0), 5);
    EXPECT_EQ(UserService::ComputeScoreLogic(0, 10, 80), 1);
}