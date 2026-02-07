//
// Created by jacks on 06/02/2026.
//

#include <gtest/gtest.h>

#include "Board.h"
#include "Fen.h"
#include "MagicBitboards.h"
#include "Piece.h"


TEST(MagicBitboard, KnightMoves){

    MagicBitboards::init();
    auto board = Board(FenHelpers::StartPos);
    auto piece = Piece(Knight, White);
    auto movesFromA1 = MagicBitboards::getMoves(piece,0,1ULL);

    EXPECT_EQ(movesFromA1,0x20400);

    auto movesfromC5 = MagicBitboards::getMoves(piece,34,1ULL);
    EXPECT_EQ(movesfromC5,0xa1100110a0000);
}

TEST(MagicBitboard, RookMoves){
    MagicBitboards::init();
    // the white rook on a1 is clear to go as far as enemy pawn
    auto board = Board("rnbqkbnr/pppppppp/8/8/8/8/1PPPPPPP/RNBQKBNR w KQkq - 0 1");
    auto moves = MagicBitboards::getMoves(Piece(Rook,White),0,board.getOccupancy());
    EXPECT_EQ(moves,282578800148738);
}

TEST(MagicBitboard, DiagonalMoves){
    MagicBitboards::init();
    auto board = Board("rnbqkbnr/pppppppp/8/8/8/8/2P1PPPP/RNBQKBNR b KQkq - 0 1");
    auto moves = MagicBitboards::getMoves(Piece(Bishop,White),2, board.getOccupancy());
    EXPECT_EQ(moves,0x804020110a00);
}

TEST(MagicBitboard, KingMoves){
    MagicBitboards::init();
    auto board = Board();
    auto kingMoves = MagicBitboards::getMoves(Piece(King,White),4, board.getOccupancy());
    EXPECT_EQ(kingMoves,0x3828);
}

TEST(MagicBitboard, PawnMoves){
MagicBitboards::init();
    auto board = Board();

    EXPECT_EQ(MagicBitboards::getMoves(Piece(Pawn,White),8,board.getOccupancy()), 0x1030000);
    EXPECT_EQ(MagicBitboards::getMoves(Piece(Pawn,Black),48,board.getOccupancy()), 0x30100000000);
}