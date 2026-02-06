//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_BOARD_H
#define ONYXCPP_BOARD_H
#include <array>
#include <string>

#include "Piece.h"
#include "types.h"


class Board {
private:

    std::array<Bitboard, 12> boards_;
    std::array<Piece, 64> board_;

public:

    Board();
    Board(Fen fen);

    Bitboard getBoardByPiece(const Piece piece) const{return boards_[piece.index()];}
    Piece pieceAtSquare(const Square square) const {return board_[square];}

    void loadFen(Fen fen);
    Fen getFen();
    void setOn(Piece piece, Square square);
    void setOff(Piece piece, Square square);
    void move(Piece piece, Square from, Square to);

private:
    void FromFen(Fen fen);

};


#endif //ONYXCPP_BOARD_H