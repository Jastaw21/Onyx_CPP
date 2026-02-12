//
// Created by jacks on 11/02/2026.
//

#include "../include/TranspositionTable.h"

#include "utils.h"

bool TTEntry::trust(const int depth_, const int alpha, const int beta, const ZobristHash hash) const{
    if (hash != key) return false;

    if (depth < depth_) return false;
    switch (bound) {
        case NONE: return false; // carry over should never happen
        case EXACT: return true;
        case UPPER_BOUND: return score <= alpha; {}
        case LOWER_BOUND: return score >= beta;
    }
    return false;
}

TranspositionTable::TranspositionTable(const int sizeInMb){
    const auto numberOfEntries = sizeInMb * 1024 * 1024 / sizeof(TTEntry);

    // next power of two bit trick
    auto v = numberOfEntries;
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    table.resize(v);
    indexMask = v - 1ULL;
}

void TranspositionTable::Store(const ZobristHash key, const Move move, const int score, const Bounds bound, const int depth, const int age){
    const uint64_t index = key & indexMask;
    const auto existingEntry = table[index];
    const TTEntry newEntry{key, move, score, bound,depth,age};

    // empty - always replace
    if (existingEntry.key == 0) {
        table[index] = newEntry;
        return;
    }

    // always replace on greater depth
    if (existingEntry.depth < depth) { table[index] = newEntry; return; }

    // always replace a new search "age"
    if (existingEntry.age != age) {table[index] = newEntry; return;}
}

TTEntry TranspositionTable::Lookup(const ZobristHash key) const{
    const uint64_t index = key & indexMask;
    return table[index];
}

TTEval TranspositionTable::Lookup(const ZobristHash key, const int depthFromRoot, const int alpha, const int beta) const{
    const uint64_t index = key & indexMask;
    auto entry = table[index];

    if (entry.key == 0) return TTEval::Failed(); // empty
    if (entry.key != key) return TTEval::Failed(); // key doesnt match
    if (entry.depth < depthFromRoot) return TTEval::Failed(); // not deep enough

    const auto mateAdjustedScore = correctedMatedScore(entry.score, depthFromRoot);
    entry.score = mateAdjustedScore;

    if (entry.bound == EXACT) return TTEval{mateAdjustedScore, true}; // exact match
    if (entry.bound == UPPER_BOUND && mateAdjustedScore <= alpha) return TTEval{mateAdjustedScore, true}; // upper bound
    if (entry.bound == LOWER_BOUND && mateAdjustedScore >= beta) return TTEval{mateAdjustedScore, true}; // lower bound

    return TTEval::Failed(); // no match;
}