//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_MAGICBITBOARDS_H
#define ONYXCPP_MAGICBITBOARDS_H

#include <vector>


#include "types.h"

struct Magic {
    Bitboard mask;
    Bitboard magic;
    int shift;
    std::vector<Bitboard> attacks;
};


class MagicBitboards {
    static Magic straightMagics[64];
    static Magic diagonalMagics[64];
    static Bitboard knightMoves[64];
    static Bitboard kingMoves[64];
    static Bitboard pawnAttacks[2][64];
    static Bitboard straightMagicNumbers[64];
    static Bitboard diagonalMagicNumbers[64];


    static void initStraightMagics();
    static void initDiagonalMagics();
    static void initKnightMoves();
    static void initKingMoves();
    static void initPawnAttacks();

    static Bitboard getStraightMask(Square from);
    static Bitboard initStraightMovesOccupancy(Square, Bitboard occupancy);
    static Bitboard initDiagonalMovesOccupancy(Square square, Bitboard occupancy);
    static Bitboard possibleOccupancyByIndex(int index, Bitboard allOccupancies);
    static Bitboard getDiagonalMask(Square from);

    static Bitboard getStraightMoves(Square from, Bitboard occupancy);
    static Bitboard getDiagonalMoves(Square from, Bitboard occupancy);

    static bool wasInit;


    static bool inRange(const int rank, const int file){ return rank >= 0 && rank <= 7 && file >= 0 && file <= 7; }

public:

    static Bitboard getMoves(struct Piece piece, Square square, Bitboard occupancy);
    static Bitboard getPawnPushes(Square from, Bitboard occupancy, bool isWhite);
    static Bitboard getPawnAttacks(Square from, Bitboard occupancy, bool isWhite);
    static void init();

};


#endif //ONYXCPP_MAGICBITBOARDS_H