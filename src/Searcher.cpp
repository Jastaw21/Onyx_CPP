//
// Created by jacks on 08/02/2026.
//

#include "../include/Searcher.h"

#include <iostream>


#include "Evaluator.h"
#include "MoveGenerator.h"
#include "MoveList.h"
#include "Referee.h"
#include "SearchController.h"



SearchResults Searcher::search(const SearchOptions& options){
    bestScore = -INF;

    bestMove = Move();
    bestScore = -INF;
    statistics_ = Statistics();

    SearchResults lastCompleted{bestScore, bestMove};

    bool foundValidMove = false;
    for (int depth = 1; depth <= options.depthLimit; ++depth) {
        pvLength[0] = 0;
        for (int i = 0; i < MAX_PLY; ++i)
            pvLength[i] = 0;

        const auto result = DoSearch(depth, 0, -INF, INF);

        if (!result.completed)
            break; // stop requested

        foundValidMove = true;
        bestScore = result.score;
        lastCompleted = {bestScore, bestMove};

        if (callback_) {
            std::string pv;
            for (int i = 0; i < pvLength[0]; ++i) {
                auto move = pvTable[0][i];
                pv += moveToNotation(move) + ' ';
            }
            callback_(SearchInfo{depth,bestScore,bestMove.Data(),statistics_,pv});
        }
    }

    if (!foundValidMove) {
        auto moves = MoveList();
        MoveGenerator::GenerateMoves(board, moves);
        for (auto move : moves) {
            if (Referee::MoveIsLegal(board, move)) {
                return {0, move};  // Return first legal move
            }
        }
    }

    return lastCompleted;
}


SearchFlag Searcher::DoSearch(const int depthRemaining, const int depthFromRoot, int alpha, const int beta){
    if (statistics_.nodes % 2047 == 0 && token_.isStopped())
        return SearchFlag::Abort();

    if (depthFromRoot > 0) {
        if (Referee::isDraw(board))
            return SearchFlag{0, true};
    }

    if (depthRemaining == 0) { const auto qEval = Quiescence(alpha, beta, depthFromRoot);
        if (!qEval.completed)
            return SearchFlag::Abort();

        return qEval;
    }

    auto hash = board.getHash();
    auto ttEntry = controller_->transpositionTable().Lookup(hash);
    auto ttEval = controller_->transpositionTable().Lookup(hash, depthRemaining, alpha, beta);

    if (ttEval.completed) {
        if (depthFromRoot == 0) {
            bestScore = ttEval.score;
            bestMove = ttEntry.move;
        }
        statistics_.hashCutoffs++;
        return SearchFlag{ttEval.score, true};
    }


    const bool isInCheck = Referee::IsInCheck(board, board.whiteToMove());

    auto moves = MoveList();
    MoveGenerator::GenerateMoves(board, moves);

    int legalMoveCount = 0;

    // by default - set upper bound. Onlu if we've improved on alpha (or get a beta cutoff) will this change
    // so if it's unchanged, we've searched all nodes, and lnow that this score is the best we can get
    Bounds storingBounds = Bounds::UPPER_BOUND;

    Move bestMoveInThisNode = Move();
    bool ttMoveTried = false;
    if (ttEntry.isValid() && Referee::MoveIsLegal(board, ttEntry.move)) {
        bestMoveInThisNode = ttEntry.move;
        ttMoveTried = true;

        statistics_.nodes++;
        pvLength[depthFromRoot + 1] = 0;
        legalMoveCount++;
        board.makeMove(ttEntry.move);
        const auto childResult = DoSearch(depthRemaining - 1, depthFromRoot + 1, -beta, -alpha);
        board.unmakeMove(ttEntry.move);

        if (!childResult.completed) return SearchFlag::Abort();

        const auto eval = -childResult.score;

        if (eval >= beta) {
            statistics_.betaCutoffs++;
            controller_->transpositionTable().Store(board.getHash(), ttEntry.move, eval, Bounds::LOWER_BOUND, depthRemaining, controller_->getAge());
            return SearchFlag{beta, true};
        }

        if (eval > alpha) {
            storingBounds = Bounds::EXACT;
            alpha = eval;
            pvTable[depthFromRoot][0] = ttEntry.move;
            for (int i = 0; i < pvLength[depthFromRoot + 1]; ++i)
                pvTable[depthFromRoot][i + 1] = pvTable[depthFromRoot + 1][i];
            pvLength[depthFromRoot] = pvLength[depthFromRoot + 1] + 1;

            if (depthFromRoot == 0) {
                bestMove = ttEntry.move;
                bestScore = eval;
            }
        }
    }

    for (auto move: moves) {
        if (ttMoveTried && move == ttEntry.move) continue;
        statistics_.nodes++;
        if (!Referee::MoveIsLegal(board, move))
            continue;

        pvLength[depthFromRoot + 1] = 0;

        legalMoveCount++;
        board.makeMove(move);
        const auto childResult = DoSearch(depthRemaining - 1, depthFromRoot + 1, -beta, -alpha);
        board.unmakeMove(move);

        if (!childResult.completed) return SearchFlag::Abort();

        const auto eval = -childResult.score;

        if (eval >= beta) {
            statistics_.betaCutoffs++;
            controller_->transpositionTable().Store(board.getHash(),move,eval,Bounds::LOWER_BOUND,depthRemaining,controller_->getAge());
            return SearchFlag{beta, true};
        }

        if (eval > alpha) {
            storingBounds = Bounds::EXACT; // we know the exact value here
            alpha = eval;
            bestMoveInThisNode = move;
            pvTable[depthFromRoot][0] = move;
            for (int i = 0; i < pvLength[depthFromRoot + 1]; ++i)
                pvTable[depthFromRoot][i + 1] = pvTable[depthFromRoot + 1][i];

            pvLength[depthFromRoot] = pvLength[depthFromRoot + 1] + 1;

            if (depthFromRoot == 0) {
                bestMove = move;
                bestScore = eval;
            }
        }
    }
    controller_->transpositionTable().Store(board.getHash(),bestMoveInThisNode,alpha,storingBounds,depthRemaining,controller_->getAge());

    // got a score
    if (legalMoveCount > 0)
        return SearchFlag{alpha, true};

    // no legal moves - checkmate
    if (isInCheck)
        return SearchFlag{-20000 + depthFromRoot, true};
    // stalemate
    return SearchFlag{0, true};
}

SearchFlag Searcher::Quiescence(int alpha, const int beta, const int depthFromRoot){
    if (statistics_.nodes % 2047 == 0 && token_.isStopped())
        return SearchFlag::Abort();

    const bool isInCheck = Referee::IsInCheck(board, board.whiteToMove());
    const auto standPat = Evaluator::Evaluate(board);

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
        const auto child = Quiescence(-beta, -alpha, depthFromRoot + 1);
        board.unmakeMove(move);
        if (!child.completed)
            return SearchFlag::Abort();
        const auto score = -child.score;

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

int Searcher::DecodeMateScore(int score, int depthFromRoot){
    if (score > MATE - 500) return score - depthFromRoot;
    if (score < -(MATE-500)) return score + depthFromRoot;
    return score;
}

int Searcher::EncodeMateScore(int score, int depthFromRoot){
    if (score > MATE - 500) return score + depthFromRoot;
    if (score < -(MATE-500)) return score - depthFromRoot;
    return score;
}

