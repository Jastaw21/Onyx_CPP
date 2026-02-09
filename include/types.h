//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_TYPES_H
#define ONYXCPP_TYPES_H
#include <cstdint>
#include <string>

using Square = int8_t;
using Bitboard = uint64_t;
using Fen = std::string;
using ZobristHash = uint64_t;

// score paid and piecesquaretable
struct sp {
    int start;
    int end;
};

using Psq =  std::array<sp,64>;

#endif //ONYXCPP_TYPES_H