//
// Created by jacks on 06/02/2026.
//

#include <gtest/gtest.h>

#include "../include/Piece.h"
#include "../include/Move.h"
#include "../include/Board.h"
#include "../include/Fen.h"

TEST(Core_Pieces, IsWhite){
    auto whitePawn = Piece(PieceType::Pawn, Colour::White);
    EXPECT_TRUE(whitePawn.isWhite());
    auto blackPawn = Piece(Pawn, Black);
    EXPECT_FALSE(blackPawn.isWhite());

    auto genericPiece = Piece();
    EXPECT_FALSE(genericPiece.exists());
}

TEST(Core_Pieces, type){
    auto whitePawn = Piece(PieceType::Pawn, Colour::White);
    EXPECT_EQ(whitePawn.type(), Pawn);
    auto whiteRook = Piece(Rook, White);
    EXPECT_EQ(whiteRook.type(), Rook);
}

TEST(Core_Pieces, Index){
    auto whitePawn = Piece(PieceType::Pawn, Colour::White);
    EXPECT_EQ(whitePawn.index(), 0);
    auto whiteRook = Piece(Rook, White);
    EXPECT_EQ(whiteRook.index(), 3);
}

TEST(Core_Move, Init){
    Square fromSquare = 8; // a2
    Square toSquare = 16; // a4
    uint8_t flags = 0;

    auto move = Move(fromSquare, toSquare, flags);
    EXPECT_EQ(move.from(), fromSquare);
    EXPECT_EQ(move.to(), toSquare);
    EXPECT_EQ(move.flags(), 0);

    // overlapping stuff doesn't break
    flags = MoveFlags::EnPassant;
    auto ep = Move(fromSquare, toSquare, flags);
    EXPECT_EQ(ep.flags(), MoveFlags::EnPassant);
    EXPECT_EQ(ep.from(), fromSquare);
    EXPECT_EQ(ep.to(), toSquare);
}

TEST(Core_Board, Init){
    auto emptyBoard = Board();
    Piece wp = Piece(Pawn, White);
    emptyBoard.setOn(wp, 0);
    Piece pieceAtA1 = emptyBoard.pieceAtSquare(0);
    EXPECT_EQ(pieceAtA1, wp);

    Bitboard wpBoard = emptyBoard.getBoardByPiece(wp);
    EXPECT_EQ(wpBoard, 1);
}

TEST(Core_Board, InitFromFen){
    auto startFen = Board(FenStrings::StartPos);
    EXPECT_EQ(startFen.getBoardByPiece(Piece(Pawn,White)), 0xff00);
    EXPECT_EQ(startFen.getBoardByPiece(Piece(Pawn,Black)), 0xff000000000000);
}

TEST(Core_Board, GetFen){
    auto startFen = Board(FenStrings::StartPos);
    auto resultFen = startFen.getFen();
    EXPECT_EQ(resultFen, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    // clang-format off
    startFen.loadFen(           "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
    EXPECT_EQ(startFen.getBoardByPiece(Piece(Pawn,White)),0x81000e700);
    EXPECT_EQ(startFen.getFen(),   "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R");
    // clang-format on
}