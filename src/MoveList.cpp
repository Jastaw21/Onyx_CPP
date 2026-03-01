//
// Created by jacks on 28/02/2026.
//

#include "MoveList.h"
#include "Board.h"
#include "MoveGenerator.h"
#include "Referee.h"


MoveList::MoveList(const Board& board, const bool capturesOnly){
    MoveGenerator::GenerateMoves(board, *this, capturesOnly);
}

void MoveList::sort(Board& board, const Move& ttMove, const Move& killer1, const Move& killer2){
    std::ranges::stable_sort(*this, [&](const Move& a, const Move& b) {
        // tt move wins first
        if (!ttMove.isNullMove() && Referee::MoveIsLegal(board, ttMove)) {
            if (a == ttMove) return true;
            if (b == ttMove) return false;
        }

        return moveScore(a, board, killer1, killer2) > moveScore(b, board, killer1, killer2);
    });
}

int MoveList::moveScore(const Move& move, const Board& board, const Move& killer1, const Move& killer2){
    int score = 0;
    if (move.isPromotion()) score += 99000;

    if (const auto capturedPiece = board.pieceAtSquare(move.to()); capturedPiece.exists()) {
        // MVV-LVA but still reward captures
        const auto pieceMoved = board.pieceAtSquare(move.from());
        score += 12000 + ((2 * pieceValues[capturedPiece.type()]) - pieceValues[pieceMoved.type()]);
    }

    if (!killer1.isNullMove() && move == killer1) score += 9000;
    if (!killer2.isNullMove() && move == killer2) score += 7000;

    return score;
}