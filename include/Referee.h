//
// Created by jacks on 07/02/2026.
//

#ifndef ONYXCPP_REFEREE_H
#define ONYXCPP_REFEREE_H
#include "Board.h"
#include "Move.h"
#include "Searcher.h"

class Referee {
public:


    static bool MoveIsLegal(Board& board, Move move);
    static bool IsInCheck(Board& board, bool forWhite);
    static bool SquareAttacked(Square square, const Board& board, bool byWhite);
    static bool isDraw(Board& board){return isRepetition(board) || board.halfMoves() >= 100;}
    static bool isRepetition(Board& board);

private:

    static bool fullLegalityCheck(Board& board, Move move);
    static bool wouldReleasePin(Square pinnedFrom, Square pinnedTo, Square kingSquare, bool isWhite, Board& board);
};


#endif //ONYXCPP_REFEREE_H