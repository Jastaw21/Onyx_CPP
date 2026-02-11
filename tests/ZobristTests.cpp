//
// Created by jacks on 11/02/2026.
//

#include <gtest/gtest.h>

#include "Zobrist.h"
#include "Board.h"


TEST(Zobrist, UndoIsStable){
    auto board = Board();

    auto hashAtInit = board.getHash();
    board.makeMove(moveFromNotation("a2a4"));
    ASSERT_NE(board.getHash(),hashAtInit);
    board.unmakeMove(moveFromNotation("a2a4"));
    ASSERT_EQ(board.getHash(),hashAtInit);
}

TEST(Zobrist, EnPassant){

    auto board = Board("rnbqkbnr/ppp1p1pp/5p2/2Pp4/8/8/PP1PPPPP/RNBQKBNR w KQkq d6 0 3");
    uint8_t flags = EnPassant;
    auto enPassantMove = Move(34, 43, flags);
    board.makeMove(enPassantMove);

    auto hashViaMove = board.getHash();

    auto boardPostMove = Board("rnbqkbnr/ppp1p1pp/3P1p2/8/8/8/PP1PPPPP/RNBQKBNR b KQkq - 0 3");
    ASSERT_EQ(boardPostMove.getHash(),hashViaMove);
}

TEST(Zobrist, Castling){
    // white can castle
    auto board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    auto hashAtInit = board.getHash();
    auto castlingMove = Move(4,6,Castling);
    board.makeMove(castlingMove);
    ASSERT_NE(board.getHash(),hashAtInit);

    auto boardPostMove = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R4RK1 b kq - 1 1");
    ASSERT_EQ(boardPostMove.getHash(),board.getHash());
}