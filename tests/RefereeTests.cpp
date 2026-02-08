//
// Created by jacks on 08/02/2026.
//

#include <gtest/gtest.h>

#include "Fen.h"
#include "Referee.h"

TEST(RefereeTests,IsCheck){
    auto startpos = Board();
    EXPECT_FALSE(Referee::IsInCheck(startpos,startpos.whiteToMove()));
    EXPECT_FALSE(Referee::IsInCheck(startpos,!startpos.whiteToMove()));

    auto inCheck = Board("8/8/8/8/8/8/2n5/K7 w - - 0 1");
    EXPECT_TRUE(Referee::IsInCheck(inCheck,inCheck.whiteToMove()));

    auto scholarCheck = Board("rnbqkbnr/ppppp1pp/8/5p1Q/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 1 2");
    EXPECT_TRUE(Referee::IsInCheck(scholarCheck,scholarCheck.whiteToMove()));
}

TEST(RefereeTests,CantMoveIntoCheck){
    auto testBoard = Board("qrn1bnrb/pppp1ppp/N7/3k4/4p3/5B2/PPPPPPPP/QR1K1NRB b - - 0 1");

    auto exposeKing = moveFromNotation("e4e3");
    auto safeKingMove = moveFromNotation("d5d6");
    auto kingMoveIntoCheck = moveFromNotation("d5c5");

    EXPECT_FALSE(Referee::MoveIsLegal(testBoard,exposeKing));
    EXPECT_TRUE(Referee::MoveIsLegal(testBoard,safeKingMove));
    EXPECT_FALSE(Referee::MoveIsLegal(testBoard,kingMoveIntoCheck));

    auto furtherTestBoard = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/5Q2/PPPBBPpP/RN2K2R w KQkq - 0 1");
    auto kingIntocheck = moveFromNotation("e1f1");
    EXPECT_FALSE(Referee::MoveIsLegal(furtherTestBoard,kingIntocheck));
}

TEST(RefereeTests,DoesntNeedDoubleOnRayToNotBePin){
    auto board = Board(FenHelpers::KiwiPeteFen);
    auto c3a4 = moveFromNotation("c3a4");
    EXPECT_TRUE(Referee::MoveIsLegal(board,c3a4));
}
TEST(RefereeTests,SquareAttacked){
    auto board = Board("r3k2r/p1ppqpb1/bn2pnN1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1");
    EXPECT_TRUE(Referee::SquareAttacked(61,board,true));

    auto fishyKingMove = moveFromNotation("e8f8");
    EXPECT_FALSE(Referee::MoveIsLegal(board,fishyKingMove));
}