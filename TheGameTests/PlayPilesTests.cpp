#include "gtest/gtest.h"
#include "PlayPiles.h" 
#include "Card.h" 


TEST(PlayPilesTest, InitialValuesAreCorrect) {
    PlayPiles piles;

    ASSERT_EQ(1, piles.GetStackValue(PlayPiles::ascend_1)) 
	<< "Teancul ascendent 1 ar trebui sa inceapa de la 1.";
    ASSERT_EQ(100, piles.GetStackValue(PlayPiles::descend_1)) 
	<< "Teancul descendent 1 ar trebui sa inceapa de la 100.";
}


TEST(PlayPilesAscending, RegularValidMove) {
    PlayPiles piles;
    piles.PlayCardOnStack(PlayPiles::ascend_1, 50); 
    Card card_51(51);
    ASSERT_TRUE(piles.IsMoveValid(PlayPiles::ascend_1, card_51)) 
	<< "51 pe 50 (Ascendent) ar trebui sa fie valid.";
}

TEST(PlayPilesAscending, BackwardsTrickValid) {
    PlayPiles piles;
    piles.PlayCardOnStack(PlayPiles::ascend_1, 50); 
    Card card_40(40); 
    ASSERT_TRUE(piles.IsMoveValid(PlayPiles::ascend_1, card_40)) 
	<< "40 pe 50 (-10 Trick) ar trebui sa fie valid.";
}

TEST(PlayPilesAscending, InvalidSmallerCard) {
    PlayPiles piles;
    piles.PlayCardOnStack(PlayPiles::ascend_1, 50); 
    Card card_49(49); 
    ASSERT_FALSE(piles.IsMoveValid(PlayPiles::ascend_1, card_49)) 
	<< "49 pe 50 (Ascendent) nu ar trebui sa fie valid.";
}

TEST(PlayPilesAscending, InvalidTrickValue) {
    PlayPiles piles;
    piles.PlayCardOnStack(PlayPiles::ascend_1, 50);
    Card card_41(41); 
    ASSERT_FALSE(piles.IsMoveValid(PlayPiles::ascend_1, card_41)) 
	<< "41 pe 50 (-9) nu ar trebui sa fie valid.";
}


TEST(PlayPilesDescending, RegularValidMove) {
    PlayPiles piles;
    piles.PlayCardOnStack(PlayPiles::descend_1, 50); 
    Card card_49(49);
    ASSERT_TRUE(piles.IsMoveValid(PlayPiles::descend_1, card_49)) 
	<< "49 pe 50 (Descendent) ar trebui sa fie valid.";
}

TEST(PlayPilesDescending, BackwardsTrickValid) {
    PlayPiles piles;
    piles.PlayCardOnStack(PlayPiles::descend_1, 50); 
    Card card_60(60); 
    ASSERT_TRUE(piles.IsMoveValid(PlayPiles::descend_1, card_60)) 
	<< "60 pe 50 (+10 Trick) ar trebui sa fie valid.";
}

TEST(PlayPilesDescending, InvalidLargerCard) {
    PlayPiles piles;
    piles.PlayCardOnStack(PlayPiles::descend_1, 50); 
    Card card_51(51);
    ASSERT_FALSE(piles.IsMoveValid(PlayPiles::descend_1, card_51)) 
	<< "51 pe 50 (Descendent) nu ar trebui sa fie valid.";
}

TEST(PlayPilesDescending, InvalidTrickValue) {
    PlayPiles piles;
    piles.PlayCardOnStack(PlayPiles::descend_1, 50); 
    Card card_59(59); 
    ASSERT_FALSE(piles.IsMoveValid(PlayPiles::descend_1, card_59)) 
	<< "59 pe 50 (+9) nu ar trebui sa fie valid.";
}


TEST(PlayPilesUtils, CardPlacementUpdatesValue) {
    PlayPiles piles;
    piles.PlayCardOnStack(PlayPiles::ascend_2, 25);
    ASSERT_EQ(25, piles.GetStackValue(PlayPiles::ascend_2)) 
	<< "Teancul nu a fost actualizat corect dupa mutare.";
}