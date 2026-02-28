//
// Created by jacks on 28/02/2026.
//

#include "MoveList.h"
#include "Board.h"
#include "MoveGenerator.h"

MoveList::MoveList(const Board& board, const bool capturesOnly){
    MoveGenerator::GenerateMoves(board, *this, capturesOnly);
}

void MoveList::sort(const Move& ttMove){
    std::ranges::sort(moves_,[&](const Move& a, const Move& b) {
        if (a == ttMove) return true;
        if (b == ttMove) return false;
        return true; // no other sorting for now
    });
}