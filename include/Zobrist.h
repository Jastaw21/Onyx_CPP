//
// Created by jacks on 11/02/2026.
//

#ifndef ONYXCPP_ZOBRIST_H
#define ONYXCPP_ZOBRIST_H
#include <cstdint>
#include <random>

#include "Board.h"

class Zobrist {
public:

    static uint64_t fromBoard(Board* board);
    static void applyMove(uint64_t& currentHash, const Move& move, Piece pieceMoved, Piece captured, Square capturedOn, uint8_t castlingPre, uint8_t
                          castlingPost, Square enPassantPre, Square enPassantPost);
    static void init();
private:
    static uint64_t whiteToMove;
    static uint64_t enPassantSquare[64];
    static uint64_t castlingRights[4]; // KQkq

    static uint64_t pieceHashes[12][64]; // hash for each piece/each square

    static std::mt19937 rng;
    static bool initialized;
};

#endif //ONYXCPP_ZOBRIST_H