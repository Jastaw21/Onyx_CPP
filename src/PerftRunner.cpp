//
// Created by jacks on 07/02/2026.
//

#include "../include/PerftRunner.h"

#include <iostream>

#include "../include/MoveGenerator.h"
#include "../include/MoveList.h"
#include "../include/Referee.h"

uint64_t PerftRunner::RunPerft(Board& board, const int depth){

    return  CountNodes(board,depth);

}

void PerftRunner::PerftDivide(Board& board, int depth){

    uint64_t totalNodes;

    auto moves = MoveList();
    MoveGenerator::GenerateMoves(board,moves);

    for (auto move : moves) {
        if (!Referee::MoveIsLegal(board,move))
            {continue;}
        bool whiteToMove = board.whiteToMove();
        board.makeMove(move);

        if (!Referee::IsInCheck(board,whiteToMove)) {
            auto nodes = CountNodes(board, depth -1);
            totalNodes += nodes;
            std::cout << moveToNotation(move) << ": " << nodes << std::endl;
        }

        board.unmakeMove(move);
    }

}

uint64_t PerftRunner::CountNodes(Board& board, const int depth){

    uint64_t nodeCount = 0;

    if (depth == 0) return 1ULL;

    auto movesInThisPosition = MoveList();
    MoveGenerator::GenerateMoves(board,movesInThisPosition);


    for (auto & move: movesInThisPosition) {
        if (!Referee::MoveIsLegal(board,move)) {
            continue;
        }

        bool whiteToMove = board.whiteToMove();
        board.makeMove(move);

        if (!Referee::IsInCheck(board,whiteToMove))
        {nodeCount += CountNodes(board, depth -1);}

        board.unmakeMove(move);
    }

    return nodeCount;
}