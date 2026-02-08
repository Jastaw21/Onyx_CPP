//
// Created by jacks on 07/02/2026.
//

#include <gtest/gtest.h>

#include "Fen.h"
#include "MagicBitboards.h"
#include "MoveGenerator.h"
#include "MoveList.h"

TEST(MoveGen,BasicTests){
    MagicBitboards::init();
    auto board = Board();

    MoveList newList = MoveList();
    MoveGenerator::GenerateMoves(board,newList);
    EXPECT_EQ(newList.size(),20);
}

TEST(MoveGen, IncludesCaptures){
    auto board = Board("rnbqkbnr/ppp1pppp/8/8/8/8/PPP1PPPP/RNBQKBNR b KQkq - 0 1");
    auto queenCapture = Move(59,3,0);

    auto moveList = MoveList();

    MoveGenerator::GenerateMoves(board,moveList);

    EXPECT_TRUE(moveList.contains(queenCapture));

}

TEST(MoveGen,IncludesEnPassant){
    auto board = Board("rnbqkbnr/ppp1p1pp/5p2/2Pp4/8/8/PP1PPPPP/RNBQKBNR w KQkq d6 0 3");
    uint8_t flags = EnPassant;
    auto enPassantMove = Move(34, 43, flags);

    auto moveList = MoveList();
    MoveGenerator::GenerateMoves(board,moveList);
    EXPECT_TRUE(moveList.contains(enPassantMove));
}

TEST(MoveGen,CantCastleThroughAttackedSquares){
    auto board = Board(FenHelpers::KiwiPeteFen);
    auto e5g6 = moveFromNotation("e5g6");
    board.makeMove(e5g6);

    auto suspectMove = moveFromNotation("e8g8");
    suspectMove.addFlag(Castling);
    auto moves = MoveList();
    MoveGenerator::GenerateMoves(board,moves);
    EXPECT_FALSE(moves.contains(suspectMove));
}

