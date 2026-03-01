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
    killerMoves.fill(std::array{Move(), Move()});

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
            for (int i = 0; i < pvLength[0]; ++i) {
                const auto move = pvTable[0][i];
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

SearchFlag Searcher::DoSearch(const int depthRemaining, const int depthFromRoot, int alpha, const int beta){
    if (statistics_.nodes % 2047 == 0 && token_.isStopped())
        return SearchFlag::Abort();

    // check to see if we can return early with a transposition table cutoff
    Move ttMove;
    if (int ttScore = 0; ProbeTT(ttMove, ttScore, depthFromRoot, depthRemaining, alpha, beta)) {
        return SearchFlag{ttScore, true};
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


    auto moves = MoveList(board, false);
    moves.sort(board, ttMove, killerMoves[depthFromRoot][0], killerMoves[depthFromRoot][1]);

    int legalMoveCount = 0;
    int allMoveCount = 0;
    Bounds storingBound = UPPER_BOUND;
    Move bestMoveInNode;

    for (auto move: moves) {
        // basic increments
        allMoveCount++;
        statistics_.nodes++;

        // as we generate pseudo legal moves need to test each one
        if (!Referee::MoveIsLegal(board, move)) continue;
        legalMoveCount++;

        // reset the pv
        pvLength[depthFromRoot + 1] = 0;

        // check if capture before move, and if in check after
        auto capturedPiece = board.pieceAtSquare(move.to());
        board.makeMove(move);
        auto isInCheckAfterMove = Referee::IsInCheck(board, board.whiteToMove());

        bool needsFullSearch = true;
        SearchFlag searchResults;

        // try a slightly reduced search
        if (!isInCheckAfterMove && depthRemaining >= 2 && !capturedPiece.exists()) {
            int reduction = 0;
            if (allMoveCount >= 5)
                reduction = 1;
            if (depthRemaining >= 4 && allMoveCount >= 8)
                reduction = 2;

            if (reduction > 0) {
                statistics_.reducedSearches++;
                searchResults = DoSearch(depthRemaining - 1 - reduction, depthFromRoot+1, -alpha -1 ,-alpha);
                auto reducedEval = -searchResults.score;
                needsFullSearch = reducedEval > alpha;
                if (needsFullSearch) statistics_.fullResearches++;
            }
        }

        if (needsFullSearch)
            searchResults = DoSearch(depthRemaining - 1, depthFromRoot + 1, -beta, -alpha);
        board.unmakeMove(move);

        if (!searchResults.completed) return SearchFlag::Abort();
        // ReSharper disable once CppObjectMemberMightNotBeInitialized
        const auto eval = -searchResults.score;

        // beta cutoff
        if (eval >= beta) {
            statistics_.betaCutoffs++;

            controller_->transpositionTable().Store(board.getHash(), move, EncodeMateScore(eval, depthFromRoot),
                                                    LOWER_BOUND, depthRemaining,
                                                    controller_->getAge());
            storeKillerMove(depthFromRoot, move);
            return SearchFlag{beta, true};
        }

        // found an improving move
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

    // handle terminal states
    int score;
    if (legalMoveCount == 0) {
        if (Referee::IsInCheck(board, board.whiteToMove())) score = -MATE + depthFromRoot;
        else score = 0;
    } else score = alpha;

    controller_->transpositionTable().Store(board.getHash(), bestMoveInNode,
                                            EncodeMateScore(score, depthFromRoot),
                                            storingBound, depthRemaining, controller_->getAge());

    return SearchFlag{score, true};
}

bool Searcher::ProbeTT(Move& outTTMove, int& outTTScore, const int depthFromRoot, const int depthRemaining,
                       const int alpha, const int beta){
    const TTEntry* tt = controller_->transpositionTable().GetEntry(board.getHash());
    if (tt != nullptr) {
        outTTMove = tt->move;
        if (tt->depth >= depthRemaining) {
            const auto adjMateScore = DecodeMateScore(tt->score, depthFromRoot);
            bool canUse = false;
            if (tt->bound == EXACT) canUse = true;
            if (tt->bound == UPPER_BOUND) canUse = adjMateScore <= alpha;
            if (tt->bound == LOWER_BOUND) canUse = adjMateScore >= beta;

            if (canUse) {
                statistics_.hashCutoffs++;
                const bool isLegal = Referee::MoveIsLegal(board, tt->move);
                if (depthFromRoot == 0 && !tt->move.isNullMove() && isLegal)
                    bestMove = tt->move;
                if (isLegal) {
                    outTTScore = adjMateScore;
                    return true;
                }
            }
        }
    }
    return false;
}

SearchFlag Searcher::Quiescence(int alpha, const int beta, const int depthFromRoot){
    if (statistics_.nodes % 2047 == 0 && token_.isStopped())
        return SearchFlag::Abort();

    const TTEntry* tt = controller_->transpositionTable().GetEntry(board.getHash());
    Move ttMove;
    if (tt != nullptr) {
        if (tt->move != Move() && Referee::MoveIsLegal(board, tt->move))
            ttMove = tt->move;
        const auto adjMateScore = DecodeMateScore(tt->score, depthFromRoot);
        bool canUse = false;
        if (tt->bound == EXACT) canUse = true;
        if (tt->bound == UPPER_BOUND) canUse = adjMateScore <= alpha;
        if (tt->bound == LOWER_BOUND) canUse = adjMateScore >= beta;

        if (canUse) {
            statistics_.hashCutoffs++;
            return SearchFlag{adjMateScore, true};
        }
    }

    const bool isInCheck = Referee::IsInCheck(board, board.whiteToMove());
    const auto standPat = Evaluator::Evaluate(board);

    if (!isInCheck) {
        if (standPat >= beta)
            return SearchFlag{beta, true};
        if (standPat > alpha)
            alpha = standPat;
    }

    auto moves = MoveList(board, !isInCheck);
    moves.sort(board, ttMove, killerMoves[depthFromRoot][0], killerMoves[depthFromRoot][1]);

    bool hasLegalMove = false;
    Move bestMoveInNode;
    Bounds storingBound = UPPER_BOUND;

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
            controller_->transpositionTable().Store(board.getHash(), move, EncodeMateScore(beta, depthFromRoot),
                                                    LOWER_BOUND, 0, controller_->getAge());
            storeKillerMove(depthFromRoot, move);
            return SearchFlag{beta, true};
        }

        if (score > alpha) {
            alpha = score;
            bestMoveInNode = move;
            storingBound = EXACT;
        }
    }

    int finalScore;
    if (isInCheck && !hasLegalMove)
        finalScore = -MATE + depthFromRoot;
    else finalScore = alpha;

    controller_->transpositionTable().Store(board.getHash(), bestMoveInNode, EncodeMateScore(finalScore, depthFromRoot),
                                            storingBound, 0, controller_->getAge());
    return SearchFlag{finalScore, true};
}

void Searcher::storeKillerMove(const int depth, const Move move){
    const auto existingEntry = killerMoves[depth][0];
    if (existingEntry.isNullMove()) { killerMoves[depth][0] = move; } else {
        killerMoves[depth][1] = existingEntry;
        killerMoves[depth][0] = move;
    }
}