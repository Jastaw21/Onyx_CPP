//
// Created by jacks on 11/02/2026.
//

#include <gtest/gtest.h>

#include "Zobrist.h"
#include "Board.h"
#include "CLIBot.h"


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

TEST(Zobrist, CastlingQueenside){
    // white can castle queenside
    auto board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    auto castlingMove = Move(4,2,Castling);
    board.makeMove(castlingMove);

    auto boardPostMove = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/2KR3R b kq - 1 1");
    ASSERT_EQ(boardPostMove.getHash(),board.getHash());
}

TEST(Zobrist,WhiteToMove){

    // at init
    auto board = Board();
    auto hashAtInit = board.getHash();
    auto blackToMoveStart = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");
    auto blackToMoveHash = blackToMoveStart.getHash();
    ASSERT_NE(hashAtInit,blackToMoveHash);

    // after move
    board = Board();
    board.makeMove(moveFromNotation("e2e4"));
    auto hashViaMove = board.getHash();
    auto boardPostMove = Board("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    auto boardMoveFlipped = Board("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
    ASSERT_EQ(hashViaMove,boardPostMove.getHash());
    ASSERT_NE(hashViaMove,boardMoveFlipped.getHash());
}

TEST(Zobrist, ConsistencyTest) {
    Board board;
    std::vector<std::string> moves = {"e2e4", "e7e5", "g1f3", "b8c6", "f1b5", "a7a6", "b5a4", "g8f6", "e1g1"};

    for (const auto& moveStr : moves) {
        Move move = moveFromNotation(moveStr);
        board.addMoveFlags(move);
        board.makeMove(move);
        ASSERT_EQ(board.getHash(), Zobrist::fromBoard(&board)) << "Hash mismatch after move " << moveStr;
    }

    // Promotion
    board.loadFen("8/4P3/8/8/8/k7/8/K7 w - - 0 1");
    Move promo = moveFromNotation("e7e8q");
    promo.addFlag(PromotionQueen);
    board.makeMove(promo);
    ASSERT_EQ(board.getHash(), Zobrist::fromBoard(&board)) << "Hash mismatch after promotion";

    // En passant
    board.loadFen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
    Move ep = moveFromNotation("e5f6");
    ep.addFlag(EnPassant);
    board.makeMove(ep);
    ASSERT_EQ(board.getHash(), Zobrist::fromBoard(&board)) << "Hash mismatch after en passant";
}

TEST(Zobrist,StaysStableInSearch){
    auto player = CliBot();
    auto hashNow = player.GetBoard().getHash();
    auto go = GoCommand{
        6,{0,0,0,0}
    };
    player.HandleCommand(go);
    auto hashAfter = player.GetBoard().getHash();
    ASSERT_EQ(hashNow,hashAfter);
}