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
    std::ranges::sort(*this, [&](const Move& a, const Move& b) {
        if (a == ttMove) { return true; }
        if (b == ttMove) { return false; }
        return moveScore(a, board) > moveScore(b, board);
    });
}

int MoveList::moveScore(const Move& move, const Board& board){
    if (move.isPromotion()) return 10000;

    if (const auto capturedPiece = board.pieceAtSquare(move.to()); capturedPiece.exists()) {
        // MVV-LVA but still reward captures
        const auto pieceMoved = board.pieceAtSquare(move.from());
        const auto score = 7 * pieceValues[capturedPiece.type()] - pieceValues[pieceMoved.type()];
        return score;
    }

    return 0;
}