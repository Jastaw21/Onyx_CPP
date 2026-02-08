//
// Created by jacks on 08/02/2026.
//

#ifndef ONYXCPP_SEARCHWORKER_H
#define ONYXCPP_SEARCHWORKER_H
#include "Board.h"
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
struct Statistics {
    uint64_t nodes = 0;
    uint64_t qNodes = 0;

    uint64_t betaCutoffs = 0;
};

class SearchWorker {
public:


    SearchResults search(SearchOptions options);
    Board& GetBoard() { return board; }
    SearchWorker() : board(Board()), bestMove(Move()), bestScore(0){}

private:
    Board board;
    bool stopFlag = false;
    Statistics statistics_{};

    Move bestMove;
    int bestScore;

    SearchFlag DoSearch(int depthRemaining, int depthFromRoot, int alpha, int beta);
    SearchFlag Quiescence(int alpha, int beta, int depthFromRoot);
    void printInfo(int depth, int bestScore, Move move);

};


#endif //ONYXCPP_SEARCHWORKER_H