//
// Created by jacks on 11/02/2026.
//

#ifndef ONYXCPP_TRANSPOSITIONTABLE_H
#define ONYXCPP_TRANSPOSITIONTABLE_H
#include <cstdint>
#include <vector>

#include "Move.h"

enum Bounds {
    LOWER_BOUND = 0,
    UPPER_BOUND = 1,
    EXACT = 2,
    NONE = 3
};
struct TTEntry {
    uint64_t key = 0;
    Move move = Move();
    int score = 0;
    Bounds bound = NONE;
    int depth = 0;
    int age = 0;

    bool isValid() const { return key != 0; }
};
class TranspositionTable {

public:
    explicit TranspositionTable(int sizeInMb);
    void Store(uint64_t key, Move move, int score, Bounds bound, int depth, int age);
    TTEntry Lookup(uint64_t key) const;

private:
    std::vector<TTEntry> table;
    uint64_t indexMask;
};


#endif //ONYXCPP_TRANSPOSITIONTABLE_H