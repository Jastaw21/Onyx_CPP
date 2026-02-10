//
// Created by jacks on 08/02/2026.
//

#ifndef ONYXCPP_SEARCHWORKER_H
#define ONYXCPP_SEARCHWORKER_H
#include <functional>

#include "Board.h"
#include "CancellationToken.h"
#include "Move.h"
#include "TimeControl.h"
static constexpr int MAX_PLY = 64;
using PVTable = Move[MAX_PLY][MAX_PLY];
using PVLength = int[MAX_PLY];

struct SearchOptions {
    int depthLimit;
    TimeControl tc;
};
struct SearchResults {
    int score;
    Move bestMove;
};
struct SearchFlag {
    int score;
    bool completed = false;

    static SearchFlag Abort(){return SearchFlag{0,false};}
};

struct SearchInfo {
    int depth; int bestScore; uint32_t Move; Statistics stats; PVTable *table; PVLength *pvLength ;
};
class Searcher {
public:
    Searcher(Board& board, CancellationToken& token, InfoCallback callback = nullptr) : board(board), token_(token), callback_(callback) {}
    SearchResults search(const SearchOptions& options);
    Board& GetBoard() const{ return board; }

private:
    Board& board;
    Statistics statistics_{};

    Move bestMove;
    int bestScore = 0;

    CancellationToken& token_;
    InfoCallback callback_;

    SearchFlag DoSearch(int depthRemaining, int depthFromRoot, int alpha, int beta);
    SearchFlag Quiescence(int alpha, int beta, int depthFromRoot);
    void printInfo(int depth, int bestScore, Move move, uint64_t elapsed) const;



    Move pvTable[MAX_PLY][MAX_PLY]{{Move()}};
    PVLength pvLength{};
};


#endif //ONYXCPP_SEARCHWORKER_H