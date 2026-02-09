//
// Created by jacks on 08/02/2026.
//

#include "../include/SearchWorker.h"

#include <iostream>


#include "Evaluator.h"
#include "MoveGenerator.h"
#include "MoveList.h"
#include "Referee.h"

static constexpr int INF = 30000;

SearchResults SearchWorker::search(SearchOptions options){
    bestScore = -INF;

    bestMove = Move();
    bestScore = -INF;
    statistics_ = Statistics();

    SearchResults lastCompleted{bestScore, bestMove};

    timer.start();
    for (int depth = 1; depth <= options.depthLimit; ++depth) {
        auto result = DoSearch(depth, 0, -INF, INF);

        if (!result.completed)
            break; // stop requested

        bestScore = result.score;
        lastCompleted = {bestScore, bestMove};

        printInfo(depth, bestScore, bestMove, timer.elapsedMs());
    }

    return lastCompleted;
}


SearchFlag SearchWorker::DoSearch(const int depthRemaining, const int depthFromRoot, int alpha, const int beta){
    if (stopFlag)
        return SearchFlag::Abort();

    if (depthFromRoot > 0) {
        if (Referee::isDraw(board))
            return SearchFlag{0, true};
    }

    if (depthRemaining == 0) {
        auto qEval = Quiescence(alpha, beta, depthFromRoot);
        if (!qEval.completed)
            return SearchFlag::Abort();

        return qEval;
    }


    bool isInCheck = Referee::IsInCheck(board, board.whiteToMove());

    auto moves = MoveList();
    MoveGenerator::GenerateMoves(board, moves);

    int legalMoveCount = 0;
    for (auto move: moves) {
        statistics_.nodes++;
        if (!Referee::MoveIsLegal(board, move))
            continue;

        legalMoveCount++;
        board.makeMove(move);
        auto childResult = DoSearch(depthRemaining - 1, depthFromRoot + 1, -beta, -alpha);
        board.unmakeMove(move);

        if (!childResult.completed) return SearchFlag::Abort();

        auto eval = -childResult.score;

        if (eval >= beta) {
            statistics_.betaCutoffs++;
            return SearchFlag{beta, true};
        }

        if (eval > alpha) {
            alpha = eval;
            if (depthFromRoot == 0) {
                bestMove = move;
                bestScore = eval;
            }
        }
    }

    // got a score
    if (legalMoveCount > 0)
        return SearchFlag{alpha, true};

    // no legal moves - checkmate
    if (isInCheck)
        return SearchFlag{-20000 + depthFromRoot, true};
    // stalemate
    return SearchFlag{0, true};
}

SearchFlag SearchWorker::Quiescence(int alpha, const int beta, const int depthFromRoot){
    if (stopFlag)
        return SearchFlag::Abort();

    bool isInCheck = Referee::IsInCheck(board, board.whiteToMove());
    auto standPat = Evaluator::Evaluate(board);

    if (!isInCheck) {
        if (standPat >= beta)
            return SearchFlag{beta, true};
        if (standPat > alpha)
            alpha = standPat;
    }


    auto moves = MoveList();
    MoveGenerator::GenerateMoves(board, moves, !isInCheck);

    bool hasLegalMove = false;

    for (const auto move: moves) {

        statistics_.nodes++;
        statistics_.qNodes++;

        if (!Referee::MoveIsLegal(board, move))
            continue;
        hasLegalMove = true;
        board.makeMove(move);
        auto child = Quiescence(-beta, -alpha, depthFromRoot + 1);
        board.unmakeMove(move);
        if (!child.completed)
            return SearchFlag::Abort();
        auto score = -child.score;

        if (score >= beta) {
            statistics_.betaCutoffs++;
            return SearchFlag{beta, true};
        }

        if (score > alpha) { alpha = score; }
    }
    if (isInCheck && !hasLegalMove)
        return SearchFlag{-20000 + depthFromRoot, true};
    return SearchFlag{alpha, true};
}

void SearchWorker::printInfo(const int depth, const int bestScore, Move move, uint64_t elpasedMs){
    std::cout << "info depth " << depth << " multipv 1 " << "score cp " << bestScore << " nodes " << statistics_.nodes
    << " nps " << (statistics_.nodes / elpasedMs) * 1000.0 << " time " << elpasedMs
            << " beta cutoff " << statistics_.betaCutoffs << std::endl;
}