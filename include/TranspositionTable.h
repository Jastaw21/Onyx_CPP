//
// Created by jacks on 11/02/2026.
//

#ifndef ONYXCPP_TRANSPOSITIONTABLE_H
#define ONYXCPP_TRANSPOSITIONTABLE_H
#include <cstdint>
#include <vector>

#include "Move.h"

enum Bounds {
    // came from a beta cutoff. I.e we know is as least as good as this score, but may be much better
    // comes from the assumption that the other player won't let us get here so our further fantastic moves that may
    // have existed further down the tree wont be allowed to get there. The score will at least exceed the stored value
    LOWER_BOUND = 0,

    // this is (I think) effectively an all node. We saerched all nodes and they didn't improve alpha
    // we know we can't beat the stored score
    UPPER_BOUND = 1,
    EXACT = 2,
    NONE = 3
};
struct TTEntry {
    ZobristHash key = 0;
    Move move = Move();
    int score = 0;
    Bounds bound = NONE;
    int depth = 0;
    int age = 0;

    bool isValid() const { return key != 0; }
    bool trust(const int depth_, const int alpha, const int beta, const ZobristHash hash) const;
};

struct TTEval {
    int score = 0;
    bool completed = false;
    static TTEval Failed(){ return TTEval{0, false}; }
};

class TranspositionTable {

public:
    explicit TranspositionTable(int sizeInMb);
    void Store(ZobristHash key, Move move, int score, Bounds bound, int depth, int age);
    TTEntry Lookup(ZobristHash key) const;
    TTEval Lookup(ZobristHash key, int depthFromRoot, int alpha, int beta) const;

private:
    std::vector<TTEntry> table;
    uint64_t indexMask;
};


#endif //ONYXCPP_TRANSPOSITIONTABLE_H