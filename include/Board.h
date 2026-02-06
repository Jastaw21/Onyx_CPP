//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_BOARD_H
#define ONYXCPP_BOARD_H
#include <array>
#include <stack>
#include <string>

#include "Piece.h"
#include "types.h"
#include "utils.h"


struct Move;

struct BoardState {
    ZobristHash hash;
    uint8_t castlingRights;
    Square enPassantSquare;
    int halfMoves;
    int fullMoves;
    Piece capturedPiece;
};


class Board {

    std::array<Bitboard, 12> boards_{0ULL};
    std::array<Piece, 64> board_{Piece()};
    bool whiteToMove_ = true;
    int halfMoves_ = 0;
    int fullMoves_ = 0;
    uint8_t castlingRights_ = 0;
    Square enPassantSquare_ = -1;
    std::stack<BoardState> history_;
    ZobristHash zobrist_ = 0ULL;

public:

    Board();
    explicit Board(const Fen& fen);


    // make/unmake
    void makeMove(const Move& move_);
    void unmakeMove(const Move& move);

    // board state
    bool whiteToMove() const{ return whiteToMove_; }
    int halfMoves() const{ return halfMoves_; }
    int fullMoves() const{ return fullMoves_; }
    uint8_t castlingRights() const{ return castlingRights_; }
    Square enPassantSquare() const{ return enPassantSquare_; }

    // occupancy
    Bitboard getBoardByPiece(const Piece piece) const{ return boards_[piece.index()]; }
    Piece pieceAtSquare(const Square square) const{ return board_[square]; }

    // fen loading/gettting
    void loadFen(const Fen& fen);
    Fen getFen() const;

    // piece movements
    void setOn(Piece piece, Square square);
    void setOff(Piece piece, Square square);
    void movePiece(Piece piece, Square from, Square to);

private:

    void FromFen(const Fen& fen);
    void pushHistory(Piece capturedPiece);
    void updateCastlingRights(Piece pieceMoved, RankAndFile moveFrom);
};


#endif //ONYXCPP_BOARD_H