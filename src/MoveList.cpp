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

void MoveList::sort(Board& board, const Move& ttMove){

    std::ranges::stable_sort(*this, [&](const Move& a, const Move& b) {
        if (!ttMove.isNullMove() && Referee::MoveIsLegal(board, ttMove)) {
            if (a == ttMove) return true;
            if (b == ttMove) return false;
        }
        return moveScore(a, board) > moveScore(b, board);
    });
}

int MoveList::moveScore(const Move& move, const Board& board){

    if (const auto capturedPiece = board.pieceAtSquare(move.to()); capturedPiece.exists()) {
        // MVV-LVA but still reward captures
        const auto pieceMoved = board.pieceAtSquare(move.from());
        const auto score =  pieceValues[capturedPiece.type()] - pieceValues[pieceMoved.type()];
        return score;
    }

    return 0;
}