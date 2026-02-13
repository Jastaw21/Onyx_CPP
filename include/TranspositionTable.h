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
    bool trust(int depth_, int alpha, int beta, ZobristHash hash) const;
};

struct TTEval {
    int score = 0;
    bool completed = false;
    static TTEval Failed(){ return TTEval{0, false}; }
};

struct TTStats {
    uint64_t attemptedStores =0;
    uint64_t empties = 0;
    uint64_t ageReplaced = 0;
    uint64_t depthReplaced = 0;

    uint64_t attemptedLookups = 0;
    uint64_t notFound = 0;
    uint64_t collisions = 0;
    uint64_t depthInsufficient = 0;

    uint64_t exact = 0;
    uint64_t lowerBound = 0;
    uint64_t upperBound = 0;

    void PrintStats();

};

class TranspositionTable {

public:
    explicit TranspositionTable(int sizeInMb);
    void Store(ZobristHash key, Move move, int score, Bounds bound, int depth, int age);
    void Store(TTEntry entry);
    TTEntry Lookup(ZobristHash key) const;
    TTEval Lookup(ZobristHash key, int depthRemaining, int alpha, int beta);
    TTEntry* GetEntry(ZobristHash key);
    void Log() {stats.PrintStats();}

private:
    std::vector<TTEntry> table;
    uint64_t indexMask;
    TTStats stats{};
};


#endif //ONYXCPP_TRANSPOSITIONTABLE_H