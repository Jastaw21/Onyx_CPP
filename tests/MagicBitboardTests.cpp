//
// Created by jacks on 06/02/2026.
//

#include <gtest/gtest.h>

#include "Board.h"
#include "Fen.h"
#include "MagicBitboards.h"
#include "Piece.h"


TEST(MagicBitboard, KnightMoves){

    auto mbb = MagicBitboards();
    auto board = Board(FenHelpers::StartPos);
    auto piece = Piece(Knight, White);
    auto movesFromA1 = mbb.getMoves(piece,0,1ULL);

    EXPECT_EQ(movesFromA1,0x20400);

    auto movesfromC5 = mbb.getMoves(piece,34,1ULL);
    EXPECT_EQ(movesfromC5,0xa1100110a0000);
}

TEST(MagicBitboard, RookMoves){
    auto mbb = MagicBitboards();
    // the white rook on a1 is clear to go as far as enemy pawn
    auto board = Board("rnbqkbnr/pppppppp/8/8/8/8/1PPPPPPP/RNBQKBNR w KQkq - 0 1");
    auto moves = mbb.getMoves(Piece(Rook,White),0,board.getOccupancy());
    EXPECT_EQ(moves,282578800148738);
}

TEST(MagicBitboard, DiagonalMoves){
    auto mbb = MagicBitboards();

    auto board = Board("rnbqkbnr/pppppppp/8/8/8/8/2P1PPPP/RNBQKBNR b KQkq - 0 1");
    auto moves = mbb.getMoves(Piece(Bishop,White),2, board.getOccupancy());
    EXPECT_EQ(moves,0x804020110a00);
}

TEST(MagicBitboard, KingMoves){
    auto mbb = MagicBitboards();
    auto board = Board();
    auto kingMoves = mbb.getMoves(Piece(King,White),4, board.getOccupancy());
    EXPECT_EQ(kingMoves,0x3828);
}