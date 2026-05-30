//
// Created by jacks on 09/02/2026.
//

#include <gtest/gtest.h>
#include <pstl/algorithm_impl.h>

#include "Evaluator.h"
#include "MagicBitboards.h"

TEST(EvaluatorTests, PieceSquareScore){
    auto rookOnA1 = Board("8/8/8/8/8/8/8/R7 w - - 0 1");
    auto rookOnA7 = Board("8/R7/8/8/8/8/8/8 w - - 0 1");

    MaterialRecord matRookonA1;
    Evaluator::PopulateMaterialRecord(rookOnA1,true,matRookonA1);
    const auto a1Ratio = actualPieceValue(matRookonA1);

    MaterialRecord matRookonA7;
    Evaluator::PopulateMaterialRecord(rookOnA7,true,matRookonA7);
    const auto a7Ratio = actualPieceValue(matRookonA7);

    auto a1 = Evaluator::PieceSquareScore(true, matRookonA1, a1Ratio);
    auto a7 = Evaluator::PieceSquareScore(true,matRookonA7, a7Ratio);
}

TEST(EvaluatorTests, MaterialAdvantage){
    auto blackAheadOnePawn = Board("rnbqkbnr/pppppppp/8/8/8/8/PP1PPPPP/RNBQKBNR b KQkq - 0 1");
    auto blackAheadOnePawnAsWhite = Board("rnbqkbnr/pppppppp/8/8/8/8/PP1PPPPP/RNBQKBNR w KQkq - 0 1");

    EXPECT_GT(Evaluator::Evaluate(blackAheadOnePawn), 0);
    EXPECT_LT(Evaluator::Evaluate(blackAheadOnePawnAsWhite), 0);
}

TEST(EvaluatorTests, KingShield){
    // equal 
    auto board = Board();
    MagicBitboards::init();
    const auto allShieldsWhite = Evaluator::KingShieldScoreByColour(true, board);

    // remove one white shielding pawn
    board.loadFen("rnbqkbnr/pppppppp/8/8/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
    const auto oneDownWhite = Evaluator::KingShieldScoreByColour(true, board);

    EXPECT_LT(oneDownWhite, allShieldsWhite);

    // remove the second
    board.loadFen("rnbqkbnr/pppppppp/8/8/8/8/PPP2PPP/RNBQKBNR b KQkq - 0 1");
    const auto twoDown = Evaluator::KingShieldScoreByColour(true, board);
    EXPECT_LT(twoDown, oneDownWhite);

    // remove the third
    board.loadFen("rnbqkbnr/pppppppp/8/8/8/8/PPP3PP/RNBQKBNR b KQkq - 0 1");
    const auto threeDown = Evaluator::KingShieldScoreByColour(true, board);
    EXPECT_LT(threeDown, twoDown);

    // remove a random, non-shielding pawn
    board.loadFen("rnbqkbnr/pppppppp/8/8/8/8/1PP3PP/RNBQKBNR b KQkq - 0 1");
    const auto randomPawn = Evaluator::KingShieldScoreByColour(true, board);
    EXPECT_EQ(randomPawn, threeDown);
}

TEST(EvaluatorTests, PassedPawns){
    auto board = Board("8/4kp2/6p1/7p/P7/2K3P1/7P/8 w - - 0 1");
    // there's one passed white pawn, on a4.


    MaterialRecord whiteMat;
    Evaluator::PopulateMaterialRecord(board,true,whiteMat);
    MaterialRecord blackMat;
    Evaluator::PopulateMaterialRecord(board,false,blackMat);

    auto whitePPScore = Evaluator::PassedPawnScore(true,whiteMat,blackMat);
    EXPECT_GT(whitePPScore,0);
}

TEST(EvaluatorTests,PassedPawsNeedAdjacentFilesEmpty){
    auto board = Board("8/2p1kp2/1p4p1/7p/P2P4/2K3P1/7P/8 w - - 0 1");
    // a4 and d4 have open files ahead, but have pawns in the adjacent files.


    MaterialRecord whiteMat;
    Evaluator::PopulateMaterialRecord(board,true,whiteMat);
    MaterialRecord blackMat;
    Evaluator::PopulateMaterialRecord(board,false,blackMat);

    auto whitePPScore = Evaluator::PassedPawnScore(true,whiteMat,blackMat);
    EXPECT_EQ(whitePPScore,0);

}