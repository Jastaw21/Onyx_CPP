//
// Created by jacks on 08/02/2026.
//

#include "Searcher.h"


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

    // iterative deepening
    for (int depth = 1; depth <= options.depthLimit; ++depth) {
        // clear the pv
        pvLength[0] = 0;
        for (int& i: pvLength)
            i = 0;

        const auto result = DoSearch(depth, 0, -INF, INF);

        if (!result.completed)
            break; // stop requested

        foundValidMove = true;
        bestScore = result.score;
        lastCompleted = {bestScore, bestMove};
        statistics_.depth = depth;

        if (callback_) {
            std::string pv;
            for (int i = 0; i < pvLength[0]; ++i) { const auto move = pvTable[0][i];
                pv += moveToNotation(move) + ' ';
            }
            callback_(SearchInfo{depth, bestScore, bestMove.Data(), statistics_, pv});
        }
    }
    bool canUse = true;
    if (!foundValidMove) {
        std::cerr << "info PANIC: No Legal Found\n";
        canUse = false;
    }
    if (bestMove.isNullMove() && canUse) {
        std::cerr << "info PANIC: Is Null Move\n";
        canUse = false;
    }
    if (canUse && moveToNotation(bestMove) == "a1a1") {
        std::cerr << "info PANIC: Is a1a1\n";
        canUse = false;
    }
    if (canUse && !Referee::MoveIsLegal(board, bestMove)) {
        std::cerr << "info PANIC: Is Illegal\n";
        canUse = false;
    }

    if (!canUse) {
        auto moves = MoveList();
        MoveGenerator::GenerateMoves(board, moves);
        for (const auto move: moves) {
            if (Referee::MoveIsLegal(board, move)) {
                return {0, move}; // Return first legal move
            }
        }
    }

    return lastCompleted;
}


SearchFlag Searcher::DoSearch(const int depthRemaining, const int depthFromRoot, int alpha, const int beta){
    if (statistics_.nodes % 2047 == 0 && token_.isStopped())
        return SearchFlag::Abort();

    const TTEntry* tt = controller_->transpositionTable().GetEntry(board.getHash());
    if (tt != nullptr && tt->depth >= depthRemaining) { const auto adjMateScore = DecodeMateScore(tt->score, depthFromRoot);
        bool canUse = false;
        if (tt->bound == EXACT) canUse = true;
        if (tt->bound == UPPER_BOUND) canUse = adjMateScore <= alpha;
        if (tt->bound == LOWER_BOUND) canUse = adjMateScore >= beta;

        if (canUse) {
            statistics_.hashCutoffs++;
            const bool isNullMove = tt->move.isNullMove();
            const bool isLegal = Referee::MoveIsLegal(board, tt->move);
            if (depthFromRoot == 0 && !isNullMove && isLegal)
                bestMove = tt->move;
            if (isLegal)
                return SearchFlag{adjMateScore, true};
            else statistics_.emptyTTMoves++;
        }
    }

    if (depthFromRoot > 0) {
        if (Referee::isDraw(board))
            return SearchFlag{0, true};
    }

    if (depthRemaining == 0) {
        const auto qEval = Quiescence(alpha, beta, depthFromRoot);
        if (!qEval.completed) return SearchFlag::Abort();
        return qEval;
    }

    const bool isInCheck = Referee::IsInCheck(board, board.whiteToMove());

    auto moves = MoveList();
    MoveGenerator::GenerateMoves(board, moves);

    int legalMoveCount = 0;
    Bounds storingBound = UPPER_BOUND;
    Move bestMoveInNode;
    for (auto move: moves) {
        statistics_.nodes++;
        if (!Referee::MoveIsLegal(board, move)) continue;

        pvLength[depthFromRoot + 1] = 0;

        legalMoveCount++;
        board.makeMove(move);
        const auto [score, completed] = DoSearch(depthRemaining - 1, depthFromRoot + 1, -beta, -alpha);
        board.unmakeMove(move);

        if (!completed) return SearchFlag::Abort();
        const auto eval = -score;

        if (eval >= beta) {
            statistics_.betaCutoffs++;

            controller_->transpositionTable().Store(board.getHash(), move, eval, LOWER_BOUND, depthRemaining,
                                                    controller_->getAge());
            return SearchFlag{beta, true};
        }

        if (eval > alpha) {
            bestMoveInNode = move;
            storingBound = EXACT;
            alpha = eval;
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

    int score;
    if (legalMoveCount == 0) {
        if (isInCheck) score = -MATE + depthFromRoot;
        else score = 0;
    } else score = alpha;

    controller_->transpositionTable().Store(board.getHash(), bestMoveInNode, score, storingBound, depthRemaining,
                                            controller_->getAge());

    return SearchFlag{score, true};
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

int Searcher::DecodeMateScore(const int score, const int depthFromRoot){
    if (score > MATE - 500) return score - depthFromRoot;
    if (score < -(MATE - 500)) return score + depthFromRoot;
    return score;
}

int Searcher::EncodeMateScore(const int score, const int depthFromRoot){
    if (score > MATE - 500) return score + depthFromRoot;
    if (score < -(MATE - 500)) return score - depthFromRoot;
    return score;
}