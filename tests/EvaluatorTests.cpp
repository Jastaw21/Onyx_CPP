//
// Created by jacks on 09/02/2026.
//

#include <gtest/gtest.h>

#include "Evaluator.h"
#include "MagicBitboards.h"

TEST(EvaluatorTests, PieceSquareScore){
    auto rookOnA1 = Board("8/8/8/8/8/8/8/R7 w - - 0 1");
    auto rookOnA7 = Board("8/R7/8/8/8/8/8/8 w - - 0 1");


    auto a1 = Evaluator::EvaluateMaterial(rookOnA1, true,0.0f).pieceSquareScore;
    auto a7 = Evaluator::EvaluateMaterial(rookOnA7, true,0.0f).pieceSquareScore;


    // prefers forward rooks
    EXPECT_TRUE(a7>a1);

    auto rookOnA1_black = Board("8/8/8/8/8/8/8/r7 b - - 0 1");
    auto rookOnA7_black = Board("8/r7/8/8/8/8/8/8 b - - 0 1");


    auto a1_black = Evaluator::EvaluateMaterial(rookOnA1_black, false,0.0f).pieceSquareScore;
    auto a7_black = Evaluator::EvaluateMaterial(rookOnA7_black, false,0.0f).pieceSquareScore;
    EXPECT_TRUE(a1_black>a7_black);
}

TEST(EvaluatorTests,MaterialAdvantage){
    auto blackAheadOnePawn = Board("rnbqkbnr/pppppppp/8/8/8/8/PP1PPPPP/RNBQKBNR b KQkq - 0 1");
    auto blackAheadOnePawnAsWhite = Board("rnbqkbnr/pppppppp/8/8/8/8/PP1PPPPP/RNBQKBNR w KQkq - 0 1");


    EXPECT_GT(Evaluator::Evaluate(blackAheadOnePawn), 0);
    EXPECT_LT(Evaluator::Evaluate(blackAheadOnePawnAsWhite), 0);
}

TEST(EvaluatorTests, KingShield) {
    // equal 
    auto board = Board();
    MagicBitboards::init();
    auto allShields = Evaluator::KingShieldScore(true, board);


    // remove one white shielder
    board.loadFen("rnbqkbnr/pppppppp/8/8/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
    auto oneDown = Evaluator::KingShieldScore(true, board);

    EXPECT_LT(oneDown, allShields);

    // remove the second
    board.loadFen("rnbqkbnr/pppppppp/8/8/8/8/PPP2PPP/RNBQKBNR b KQkq - 0 1");
    auto twoDown = Evaluator::KingShieldScore(true, board);
    EXPECT_LT(twoDown, oneDown);

    // remove the third
    board.loadFen("rnbqkbnr/pppppppp/8/8/8/8/PPP3PP/RNBQKBNR b KQkq - 0 1");
    auto threeDown = Evaluator::KingShieldScore(true, board);
    EXPECT_LT(threeDown, twoDown);

    // remove a random, non shielding pawn
    board.loadFen("rnbqkbnr/pppppppp/8/8/8/8/1PP3PP/RNBQKBNR b KQkq - 0 1");
    auto randomPawn = Evaluator::KingShieldScore(true, board);
    EXPECT_EQ(randomPawn, threeDown);
}