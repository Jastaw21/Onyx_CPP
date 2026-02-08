//
// Created by jacks on 08/02/2026.
//

#include "../include/Evaluator.h"
int Evaluator::Evaluate(Board& board){

    Bitboard whiteCount = board.getOccupancy(White);
    Bitboard blackCount = board.getOccupancy(Black);

    auto pieceCount = whiteCount - blackCount;

    return  pieceCount * board.whiteToMove() ? 1 : -1;
 }