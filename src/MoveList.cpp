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
	const bool ttMoveValid = !ttMove.isNullMove() && Referee::MoveIsLegal(board, ttMove);
    const bool killer1Valid = !killer1.isNullMove();
	const bool killer2Valid = !killer2.isNullMove();

    std::ranges::stable_sort(*this, [&](const Move& a, const Move& b) {
        // tt move wins first
        if (ttMoveValid) {
            if (a == ttMove) return true;
            if (b == ttMove) return false;
        }

        // only call the killer sorting if they're valid moves, avoid multiple calls to isnull if not, once inside sort
        if (killer1Valid && killer2Valid) {
			return moveScore(a, board, killer1, killer2) > moveScore(b, board, killer1, killer2);
        }
		return moveScore(a, board) > moveScore(b, board);
       
    });
}


int MoveList::moveScore(const Move& move, const Board& board) {
    int score = 0;
    if (move.isPromotion()) score += 99000;
    if (const auto capturedPiece = board.pieceAtSquare(move.to()); capturedPiece.exists()) {
        // MVV-LVA but still reward captures
        const auto pieceMoved = board.pieceAtSquare(move.from());
        score += 12000 + (2 * pieceValues[capturedPiece.type()] - pieceValues[pieceMoved.type()]);
    }
    return score;
}

int MoveList::moveScore(const Move& move, const Board& board, const Move& killer1, const Move& killer2){
    int score = 0;
    if (move.isPromotion()) score += 99000;

    if (const auto capturedPiece = board.pieceAtSquare(move.to()); capturedPiece.exists()) {
        // MVV-LVA but still reward captures
        const auto pieceMoved = board.pieceAtSquare(move.from());
        score += 12000 + (2 * pieceValues[capturedPiece.type()] - pieceValues[pieceMoved.type()]);
    }

    if (move == killer1) score += 9000;
    if (move == killer2) score += 7000;

    return score;
}