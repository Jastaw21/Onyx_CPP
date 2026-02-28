//
// Created by jacks on 06/02/2026.
//

#include <gtest/gtest.h>

#include "Board.h"
#include "CLIBot.h"
#include "Fen.h"
#include "Move.h"
#include "Piece.h"

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
    auto emptyBoard = Board("8/8/8/8/8/8/8/8 w - - 0 1");
    Piece wp = Piece(Pawn, White);
    emptyBoard.setOn(wp, 0);
    Piece pieceAtA1 = emptyBoard.pieceAtSquare(0);
    EXPECT_EQ(pieceAtA1, wp);

    Bitboard wpBoard = emptyBoard.getBoardByPiece(wp);
    EXPECT_EQ(wpBoard, 1);
}

TEST(Core_Board, InitFromFen){
    auto startFen = Board(FenHelpers::StartPos);

    // pawns in the right place
    EXPECT_EQ(startFen.getBoardByPiece(Piece(Pawn,White)), 0xff00);
    EXPECT_EQ(startFen.getBoardByPiece(Piece(Pawn,Black)), 0xff000000000000);

    // castling right
    EXPECT_EQ(startFen.castlingRights(),
              FenHelpers::CastlingRights::BlackKingside | FenHelpers::BlackQueenside |FenHelpers::WhiteQueenside |
              FenHelpers::WhiteKingside);

    EXPECT_EQ(startFen.enPassantSquare(), -1);
    EXPECT_EQ(startFen.halfMoves(), 0);
    EXPECT_EQ(startFen.fullMoves(), 1);
}

TEST(Core_Board, GetFen){
    auto startFen = Board(FenHelpers::StartPos);
    auto resultFen = startFen.getFen();
    EXPECT_EQ(resultFen, FenHelpers::StartPos);

    // clang-format off
    startFen.loadFen(           "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    EXPECT_EQ(startFen.getBoardByPiece(Piece(Pawn,White)),0x81000e700);
    EXPECT_EQ(startFen.getFen(),   "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    // clang-format on
}

TEST(Core_Square, NotationToSquare){
    auto a1 = notationToSquare("a1");
    EXPECT_EQ(a1, 0);
    auto a2 = notationToSquare("a2");
    EXPECT_EQ(a2, 8);
    auto h8 = notationToSquare("h8");
    EXPECT_EQ(h8, 63);
}

TEST(Core_MakeMove, BasicPush){
    auto board = Board(FenHelpers::StartPos);

    // push the a-pawn forwards - it should all update
    auto pawnPush = Move(8, 16, 0);
    board.makeMove(pawnPush);
    EXPECT_EQ(board.getFen(), "rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1");

    // undo it, and should be back to the start
    board.unmakeMove(pawnPush);
    EXPECT_EQ(board.getFen(), FenHelpers::StartPos);
}

TEST(Core_MakeMove, PawnDoublePush){
    auto board = Board(FenHelpers::StartPos);

    // double push the pawn - should get an en passant square on a3
    auto pawnDoublePush = Move(8, 24, 0);
    board.makeMove(pawnDoublePush);
    EXPECT_EQ(board.getFen(), "rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR b KQkq a3 0 1");

    // unmaking it should fully reset
    board.unmakeMove(pawnDoublePush);
    EXPECT_EQ(board.getFen(), FenHelpers::StartPos);
}

TEST(Core_MakeMove, LossOfEnPassantSquare){
    auto board = Board(FenHelpers::StartPos);

    // double push the pawn - should get an en passant square on a3
    auto pawnDoublePush = Move(8, 24, 0);
    board.makeMove(pawnDoublePush);

    // black goes a7a6, ep should go
    auto blackResponse = Move(48, 40, 0);
    board.makeMove(blackResponse);
    EXPECT_EQ(board.getFen(), "rnbqkbnr/1ppppppp/p7/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq - 0 2");

    // but undoing this move gives us en passant square back
    board.unmakeMove(blackResponse);
    EXPECT_EQ(board.getFen(), "rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR b KQkq a3 0 1");
}

TEST(Core_MakeMove, ExecuteEnPassant){
    // white can en passant on d6
    auto board = Board("rnbqkbnr/ppp1p1pp/5p2/2Pp4/8/8/PP1PPPPP/RNBQKBNR w KQkq d6 0 3");

    // capturing via en passant properly captures
    uint8_t flags = EnPassant;
    auto enPassantMove = Move(34, 43, flags);
    board.makeMove(enPassantMove);
    EXPECT_EQ(board.getFen(), "rnbqkbnr/ppp1p1pp/3P1p2/8/8/8/PP1PPPPP/RNBQKBNR b KQkq - 0 3");

    // and undo fully restores.
    board.unmakeMove(enPassantMove);
    EXPECT_EQ(board.getFen(), "rnbqkbnr/ppp1p1pp/5p2/2Pp4/8/8/PP1PPPPP/RNBQKBNR w KQkq d6 0 3");
}

TEST(Core_MakeMove, BasicCapture){
    // the white queen can capture the knight on f6
    auto board = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    // making the capture should remove the captured piece
    auto capture = Move(21, 45, 0);
    board.makeMove(capture);
    EXPECT_EQ(board.getFen(), "r3k2r/p1ppqpb1/bn2pQp1/3PN3/1p2P3/2N4p/PPPBBPPP/R3K2R b KQkq - 0 1");

    // and undoing should be a full reset
    board.unmakeMove(capture);
    EXPECT_EQ(board.getFen(), "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
}

TEST(Core_MakeMove, LoseCastling){
    auto fen = "r3k2r/p1ppqpb1/bn2pQp1/3PN3/1p2P3/2N4p/PPPBBPPP/R3K2R w KQkq - 0 1";

    auto board = Board(fen);
    auto kingMove = Move(4, 5, 0);
    board.makeMove(kingMove);

    // we now have lost all white castling rights
    EXPECT_EQ(board.getFen(), "r3k2r/p1ppqpb1/bn2pQp1/3PN3/1p2P3/2N4p/PPPBBPPP/R4K1R b kq - 1 1");

    // but can undo it
    board.unmakeMove(kingMove);
    EXPECT_EQ(board.getFen(), fen);

    // can lose just queenside
    board.makeMove(Move(0, 1, 0));
    EXPECT_EQ(board.getFen(), "r3k2r/p1ppqpb1/bn2pQp1/3PN3/1p2P3/2N4p/PPPBBPPP/1R2K2R b Kkq - 1 1");
    board.unmakeMove(Move(0, 1, 0));
    EXPECT_EQ(board.getFen(), fen);

    // and just kingside
    board.makeMove(Move(7, 6, 0));
    EXPECT_EQ(board.getFen(), "r3k2r/p1ppqpb1/bn2pQp1/3PN3/1p2P3/2N4p/PPPBBPPP/R3K1R1 b Qkq - 1 1");
    board.unmakeMove(Move(7, 6, 0));
    EXPECT_EQ(board.getFen(), fen);

    auto kpBoard = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/Np2P3/5Q1p/PPPBBPPP/R3K2R b KQkq - 0 1");
    auto castlingRights = kpBoard.castlingRights();
    auto rookMove = moveFromNotation("h8h4");
    kpBoard.makeMove(rookMove);
    EXPECT_FALSE(kpBoard.castlingRights() == castlingRights);
    EXPECT_FALSE(kpBoard.castlingRights() & FenHelpers::CastlingRights::BlackKingside);
}

TEST(Core_MakeMove, ApplyCastling){
    auto startingFen = "r3k2r/p1ppqpb1/bn2pQp1/3PN3/1p2P3/2N4p/PPPBBPPP/R3K2R w KQkq - 0 1";
    auto board = Board(startingFen);

    // white can castle either side
    uint8_t flags = Castling;
    auto castlingMove = Move(4, 6, flags);
    board.makeMove(castlingMove);
    EXPECT_EQ(board.getFen(), "r3k2r/p1ppqpb1/bn2pQp1/3PN3/1p2P3/2N4p/PPPBBPPP/R4RK1 b kq - 1 1");
    board.unmakeMove(castlingMove);
    EXPECT_EQ(board.getFen(), startingFen);
}

TEST(Core_MakeMove, ApplyPromotion){
    // the white pawn on d7 can promote
    auto board = Board("r5kr/p1QP1p2/bn2pb2/2p1N1p1/1p2P3/2N4p/PPPBBPPP/2KR3R w - - 0 7");
    auto flags = MoveFlags::PromotionQueen;
    auto promotionMove = Move(51, 59, flags);
    board.makeMove(promotionMove);
    EXPECT_EQ(board.getFen(), "r2Q2kr/p1Q2p2/bn2pb2/2p1N1p1/1p2P3/2N4p/PPPBBPPP/2KR3R b - - 0 7");

    board.unmakeMove(promotionMove);
    EXPECT_EQ(board.getFen(), "r5kr/p1QP1p2/bn2pb2/2p1N1p1/1p2P3/2N4p/PPPBBPPP/2KR3R w - - 0 7");
}

TEST(Core_MakeMove, ApplyPromotionWithCapture){
    auto board = Board("2r3kr/p1QP1p2/bn2pb2/2p1N1p1/1p2P3/1PN4p/P1PBBPPP/2KR3R w - - 1 8");
    auto flags = MoveFlags::PromotionQueen;
    auto promotionMove = Move(51, 58, flags);
    board.makeMove(promotionMove);
    EXPECT_EQ(board.getFen(), "2Q3kr/p1Q2p2/bn2pb2/2p1N1p1/1p2P3/1PN4p/P1PBBPPP/2KR3R b - - 0 8");

    board.unmakeMove(promotionMove);
    EXPECT_EQ(board.getFen(), "2r3kr/p1QP1p2/bn2pb2/2p1N1p1/1p2P3/1PN4p/P1PBBPPP/2KR3R w - - 1 8");
}

TEST(CoreTests, BuildBoardFromMoveString2){
    auto bot = CliBot();
    auto parser = UCIParser();
    auto moveString = "position fen rn1qkb1r/pppbpppp/1n6/3pP3/3P3N/7P/PPP2PP1/RNBQKB1R w KQkq - 0 1 moves b1c3 b8c6 h4f3 d7f5 e1d2 b6c4 f1c4 d5c4 d4d5 f7f6 g2g4 f5g6 e5e6 c6b4 d1e2 b4c2 a1b1 g6d3 e2d1 c2d4 b1a1 d8d6 h1e1 d4c2 d1c2 d3c2 d2c2 c7c6 f3d4 c6d5 c1e3 a7a5 a2a4 g7g5 c3b5 d6c5 d4f5 c5c6 b5d4 c6c5 a1a3 c5b4 e3d2 b4c5 e1e3 e8c8 d4b5";
    auto command = parser.parse(moveString);
    bot.HandleCommand(command.value());
    auto actualFen = bot.GetBoard().getFen();
    auto expectedFen = "2kr1b1r/1p2p2p/4Pp2/pNqp1Np1/P1p3P1/R3R2P/1PKB1P2/8 b - - 13 24";
    EXPECT_EQ(actualFen, expectedFen);


}