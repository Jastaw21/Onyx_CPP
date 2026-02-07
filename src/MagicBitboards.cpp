//
// Created by jacks on 06/02/2026.
//

#include "../include/MagicBitboards.h"

#include "../include/utils.h"


void MagicBitboards::initStraightMagics(){
    for (int square = 0; square < 64; square++) {
        Magic& magic = straightMagics[square];
        magic.mask = getStraightMask(square);
        magic.magic = straightMagicNumbers[square];
        magic.shift = 64 - std::popcount(magic.mask);
        const int attacksSize = 1 << std::popcount(magic.mask);

        magic.attacks.resize(attacksSize);

        for (int i = 0; i < attacksSize; i++) {
            Bitboard possibleOccupancy = possibleOccupancyByIndex(i, magic.mask);
            auto moves = initStraightMovesOccupancy(square, possibleOccupancy);

            possibleOccupancy *= magic.magic; // apply the magic number
            possibleOccupancy >>= magic.shift; // shift it down
            magic.attacks[possibleOccupancy] = moves;
        }
    }
}

void MagicBitboards::initDiagonalMagics(){
    for (int square = 0; square < 64; square++) {
        Magic& magic = diagonalMagics[square];
        magic.mask = getDiagonalMask(square);
        magic.magic = diagonalMagicNumbers[square];
        magic.shift = 64 - std::popcount(magic.mask);
        const int attacksSize = 1 << std::popcount(magic.mask);

        magic.attacks.resize(attacksSize);

        for (int i = 0; i < attacksSize; i++) {
            Bitboard possibleOccupancy = possibleOccupancyByIndex(i, magic.mask);
            auto moves = initDiagonalMovesOccupancy(square, possibleOccupancy);

            possibleOccupancy *= magic.magic; // apply the magic number
            possibleOccupancy >>= magic.shift; // shift it down
            magic.attacks[possibleOccupancy] = moves;
        }
    }
}

void MagicBitboards::initKnightMoves(){
    struct KnightMove {
        int deltaR;
        int deltaF;
    };

    KnightMove allKnightMoves[8] = {
                KnightMove{2, 1}, KnightMove{2, -1},
                KnightMove{1, 2}, KnightMove{1, -2},
                KnightMove{-1, 2}, KnightMove{-1, -2},
                KnightMove{-2, 1}, KnightMove{-2, -1}
            };

    for (int i = 0; i < 64; i++) {
        Bitboard possibleMoves = 0ULL;
        for (const auto knightMove: allKnightMoves) {
            const auto rankAndFile = squareToRankAndFile(i);
            const auto newRank = rankAndFile.rank + knightMove.deltaR;
            const auto newFile = rankAndFile.file + knightMove.deltaF;
            if (!inRange(newRank, newFile)) { continue; }

            possibleMoves |= 1ULL << rankAndFileToSquare(newRank, newFile);
        }
        knightMoves[i] = possibleMoves;
    }
}

void MagicBitboards::initKingMoves(){
    RankAndFile kingDirections[8]
            = {
                RankAndFile{1, 0}, RankAndFile{1, 1},
                RankAndFile{0, 1}, RankAndFile{-1, 1},
                RankAndFile{-1, 0}, RankAndFile{-1, -1},
                RankAndFile{0, -1}, RankAndFile{1, -1}
            };
    for (int i = 0; i < 64; i++) {
        Bitboard possibleMoves = 0ULL;
        for (const auto kingMove: kingDirections) {
            const auto rankAndFile = squareToRankAndFile(i);
            const auto newRank = rankAndFile.rank + kingMove.rank;
            const auto newFile = rankAndFile.file + kingMove.file;
            if (!inRange(newRank, newFile)) { continue; }

            possibleMoves |= 1ULL << rankAndFileToSquare(newRank, newFile);
        }
        kingMoves[i] = possibleMoves;
    }
}

Bitboard MagicBitboards::getStraightMask(Square from){
    const auto raf = squareToRankAndFile(from);

    Bitboard mask = 0ULL;

    for (int f = 1; f < 7; f++) {
        if (f == raf.file) continue;
        const auto square = rankAndFileToSquare(raf.rank, f);
        mask |= 1ULL << square;
    }
    for (int r = 1; r < 7; r++) {
        if (r == raf.rank) continue;
        const auto square = rankAndFileToSquare(r, raf.file);
        mask |= 1ULL << square;
    }

    return mask;
}

Bitboard MagicBitboards::initStraightMovesOccupancy(Square square, Bitboard occupancy){
    const auto raf = squareToRankAndFile(square);

    Bitboard moves = 0ULL;

    // go left
    for (int i = raf.file - 1; i >= 0; i--) {
        const auto value = 1ULL << rankAndFileToSquare(raf.rank, i);
        moves |= value;
        if (occupancy & value) break;
    }
    // go right
    for (int i = raf.file + 1; i < 8; i++) {
        const auto value = 1ULL << rankAndFileToSquare(raf.rank, i);
        moves |= value;
        if (occupancy & value) break;
    }
    // go down
    for (int i = raf.rank - 1; i >= 0; i--) {
        const auto value = 1ULL << rankAndFileToSquare(i, raf.file);
        moves |= value;
        if (occupancy & value) break;
    }
    // go up
    for (int i = raf.rank + 1; i < 8; i++) {
        const auto value = 1ULL << rankAndFileToSquare(i, raf.file);
        moves |= value;
        if (occupancy & value) break;
    }

    return moves;
}

Bitboard MagicBitboards::initDiagonalMovesOccupancy(Square square, Bitboard occupancy){
    const auto raf = squareToRankAndFile(square);

    struct Step {
        int deltaR;
        int deltaF;
    };
    Bitboard mask = 0ULL;
    Step directions[4] = {Step{1, 1}, Step{1, -1}, Step{-1, -1}, Step{-1, 1}};

    for (const auto dir: directions) {
        auto newRank = raf.rank + dir.deltaR;
        auto newFile = raf.file + dir.deltaF;

        while (newRank >= 0 && newRank <= 7 && newFile >= 0 && newFile <= 7) {
            auto square = rankAndFileToSquare(newRank, newFile);
            auto value = 1ULL << square;
            mask |= value;
            if (value & occupancy) break;

            newRank += dir.deltaR;
            newFile += dir.deltaF;
        }
    }

    return mask;
}

Bitboard MagicBitboards::possibleOccupancyByIndex(const int index, Bitboard allOccupancies){
    const auto numberOfSetBits = std::popcount(allOccupancies);
    Bitboard thisOccupancyCombo = 0ULL;

    for (int i = 0; i < numberOfSetBits; i++) {
        const int lowestBit = std::countr_zero(allOccupancies);
        allOccupancies &= allOccupancies - 1; // get rid of the lowest set bit in the mask

        if (index & 1 << i) {
            // if this index is set in the mask, add it to the bits
            thisOccupancyCombo |= 1ULL << lowestBit;
        }
    }

    return thisOccupancyCombo;
}

Bitboard MagicBitboards::getDiagonalMask(Square from){
    const auto raf = squareToRankAndFile(from);

    struct Step {
        int deltaR;
        int deltaF;
    };
    Bitboard mask = 0ULL;
    Step directions[4] = {Step{1, 1}, Step{1, -1}, Step{-1, -1}, Step{-1, 1}};

    for (const auto dir: directions) {
        auto newRank = raf.rank + dir.deltaR;
        auto newFile = raf.file + dir.deltaF;

        while (newRank >= 1 && newRank <= 6 && newFile >= 1 && newFile <= 6) {
            auto square = rankAndFileToSquare(newRank, newFile);
            mask |= 1ULL << square;

            newRank += dir.deltaR;
            newFile += dir.deltaF;
        }
    }

    return mask;
}

Bitboard MagicBitboards::getStraightMoves(Square from, Bitboard occupancy){
    const Magic& magic = straightMagics[from];
    occupancy &= magic.mask;
    occupancy *= magic.magic;
    occupancy >>= magic.shift;
    return magic.attacks[occupancy];
}

Bitboard MagicBitboards::getDiagonalMoves(Square from, Bitboard occupancy){
    const Magic& magic = diagonalMagics[from];
    occupancy &= magic.mask;
    occupancy *= magic.magic;
    occupancy >>= magic.shift;
    return magic.attacks[occupancy];
}

Bitboard MagicBitboards::getMoves(Piece piece, Square square, Bitboard occupancy){
    switch (piece.type()) {
        case Knight: return knightMoves[square];
        case Queen: return getStraightMoves(square, occupancy) | getDiagonalMoves(square, occupancy);
        case Rook: return getStraightMoves(square, occupancy);
        case Bishop: return getDiagonalMoves(square, occupancy);
        case King : return kingMoves[square];
    }

    return 0ULL;
}

MagicBitboards::MagicBitboards(){
    initDiagonalMagics();
    initStraightMagics();
    initKnightMoves();
    initKingMoves();
}