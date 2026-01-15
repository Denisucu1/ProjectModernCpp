#include "gtest/gtest.h"
#include "MatchService.h"

TEST(MatchmakingLogic, ScoreRangeValidation) {

    EXPECT_TRUE(MatchService::IsInMatchmakingRange(3.5f, 3.0f));
    EXPECT_TRUE(MatchService::IsInMatchmakingRange(2.1f, 3.0f));

    EXPECT_FALSE(MatchService::IsInMatchmakingRange(4.5f, 3.0f));
    EXPECT_FALSE(MatchService::IsInMatchmakingRange(1.5f, 3.0f));
}

TEST(MatchmakingLogic, BoundaryConditions) {
    EXPECT_TRUE(MatchService::IsInMatchmakingRange(4.0f, 3.0f));
    EXPECT_TRUE(MatchService::IsInMatchmakingRange(2.0f, 3.0f));
}