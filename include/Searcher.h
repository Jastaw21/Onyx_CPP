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

};


#endif //ONYXCPP_SEARCHWORKER_H