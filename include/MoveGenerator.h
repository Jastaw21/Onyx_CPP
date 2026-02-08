//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_MOVEGENERATOR_H
#define ONYXCPP_MOVEGENERATOR_H

#include "Board.h"


class MoveList;

enum PromotionMode{
    All, QueenAndKnight
};

class MoveGenerator {

public:
    static void GenerateMoves(const Board& board, MoveList& moveSpan, bool capturesOnly = false);
    static PromotionMode promotionMode;

private:
    static void generateBasicMoves(const Board& board, MoveList& moveSpan, Bitboard us, Bitboard them, Bitboard theirKing, bool capturesOnly = false);
    static void generatePawnMoves(const Board& board, MoveList& moveSpan, Bitboard us, Bitboard them, Bitboard theirKing, bool capturesOnly = false);
    static void addCastlingmoves(const Board& board, MoveList& moveSpan, Bitboard us, Bitboard them);

    static  std::array<Piece,6> whitePieces ;
    static std::array<Piece,6>  blackPieces ;

};


#endif //ONYXCPP_MOVEGENERATOR_H