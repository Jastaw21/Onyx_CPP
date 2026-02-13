//
// Created by jacks on 11/02/2026.
//

#include "../include/TranspositionTable.h"

#include <iostream>

#include "utils.h"

bool TTEntry::trust(const int depth_, const int alpha, const int beta, const ZobristHash hash) const{
    if (hash != key) return false;

    if (depth < depth_) return false;
    switch (bound) {
        case NONE: return false; // carry over should never happen
        case EXACT: return true;
        case UPPER_BOUND: return score <= alpha;
        case LOWER_BOUND: return score >= beta;
    }
    return false;
}

void TTStats::PrintStats(){
    std::string result;

    result += "TTStats\n";
    result += "Attempted Stores: ";
    result += std::to_string(attemptedStores);
    result += ", ";

    result += "Empties: ";
    result += std::to_string(empties);
    result += ", ";

    result += "Age Replaced: ";
    result += std::to_string(ageReplaced);
    result += ", ";

    result += "Depth Replaced: ";
    result += std::to_string(depthReplaced);
    result += ", ";

    result += "Attempted Lookups: ";
    result += std::to_string(attemptedLookups);
    result += ", ";

    result += "Not Found: ";
    result += std::to_string(notFound);
    result += ", ";

    result += "Collisions: ";
    result += std::to_string(collisions);
    result += ", ";

    result += "Depth Misses: ";
    result += std::to_string(depthInsufficient);
    result += ", ";

    result += "Exact: ";
    result += std::to_string(exact);
    result += ", ";

    result += "lower: ";
    result += std::to_string(lowerBound);
    result += ", ";

    result += "upper: ";
    result += std::to_string(upperBound);
    result += ", ";

    result += "\n";

    std::cerr << result;
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

void TranspositionTable::Store(const ZobristHash key, const Move move, int score, const Bounds bound, const int depth,
                               const int age){
    const uint64_t index = key & indexMask;
    const auto existingEntry = table[index];
    stats.attemptedStores++;

    const TTEntry newEntry{key, move, score, bound, depth, age};

    // empty - always replace
    if (existingEntry.key == 0) {
        table[index] = newEntry;
        stats.empties++;
        return;
    }

    // always replace on greater depth
    if (existingEntry.depth < depth) {
        table[index] = newEntry;
        stats.depthReplaced++;
        return;
    }

    // always replace a new search "age"
    if (existingEntry.age != age) {
        stats.ageReplaced++;
        table[index] = newEntry;
        return;
    }
}

void TranspositionTable::Store(TTEntry entry){ Store(entry.key, entry.move, entry.score, entry.bound, entry.depth, entry.age); }

TTEntry TranspositionTable::Lookup(const ZobristHash key) const{
    const uint64_t index = key & indexMask;
    auto entry = table[index];
    if (entry.key != key) return TTEntry{0, Move(), 0, NONE, 0, 0};
    return entry;
}

TTEval TranspositionTable::Lookup(const ZobristHash key, const int depthRemaining, const int alpha,
                                  const int beta){
    const uint64_t index = key & indexMask;
    auto entry = table[index];

    stats.attemptedLookups++;

    // empty
    if (entry.key == 0) {
        stats.notFound++;
        return TTEval::Failed();
    }

    // key doesnt match
    if (entry.key != key) {
        stats.collisions++;
        return TTEval::Failed();
    }

    // not deep enough
    if (entry.depth < depthRemaining) {
        stats.depthInsufficient++;
        return TTEval::Failed();
    }

    const auto score = entry.score;

    // exact match
    if (entry.bound == EXACT) {
        stats.exact++;
        return TTEval{score, true};
    }
    // upper bound
    if (entry.bound == UPPER_BOUND && score <= alpha) {
        stats.upperBound++;
        return TTEval{score, true};
    }
    // lower bound
    if (entry.bound == LOWER_BOUND && score >= beta) {
        stats.lowerBound++;
        return TTEval{score, true};
    }

    return TTEval::Failed(); // no match;
}

TTEntry* TranspositionTable::GetEntry(ZobristHash key){
    const uint64_t index = key & indexMask;
    if (key == table[index].key) return &table[index];
    return nullptr;
}