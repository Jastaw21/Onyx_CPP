//
// Created by jacks on 28/02/2026.
//

#include "MoveList.h"
#include "Board.h"
#include "MoveGenerator.h"

MoveList::MoveList(const Board& board, const bool capturesOnly){
    MoveGenerator::GenerateMoves(board, *this, capturesOnly);
}

void MoveList::sort(const Board& board, const Move& ttMove){
    if (ttMove.isNullMove()) return;
    std::ranges::sort(moves_, [&](const Move& a, const Move& b) {
        if ( a == ttMove) {
            return true;
        }
        if (b == ttMove) {
            return false;
        }

        return moveScore(a, board) > moveScore(b, board);

    });
}

int MoveList::moveScore(const Move& move, const Board& board) const{
    if (move.isPromotion()) return 10000;

    auto capturedPiece = board.pieceAtSquare(move.to());
    if (capturedPiece.exists()) {
        return 8000;
    }

    if (move.flags() & Castling)
        return 6000;

    if (move.flags() & EnPassant)
        return 5000;
    return 0;
}