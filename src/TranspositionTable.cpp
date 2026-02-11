//
// Created by jacks on 11/02/2026.
//

#include "../include/TranspositionTable.h"
TranspositionTable::TranspositionTable(const int sizeInMb){
    auto numberOfEntries = sizeInMb * 1024 * 1024 / sizeof(TTEntry);

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

void TranspositionTable::Store(const uint64_t key, const Move move, const int score, const Bounds bound, const int depth, const int age){
    uint64_t index = key & indexMask;
    auto existingEntry = table[index];
    TTEntry newEntry{key, move, score, bound,depth,age};

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

TTEntry TranspositionTable::Lookup(const uint64_t key) const{
    uint64_t index = key & indexMask;
    return table[index];
}