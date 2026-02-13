//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_TYPES_H
#define ONYXCPP_TYPES_H
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>

struct SearchInfo;
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


struct Statistics {
    uint64_t nodes = 0;
    uint64_t qNodes = 0;
    uint64_t betaCutoffs = 0;
    int hashCutoffs = 0;

    void Log() const{
        std::cerr << "Nodes: " << std::to_string(nodes) << "QNodes: " << std::to_string(qNodes) << "betaCutoffs: " << std::to_string(betaCutoffs) << "hash cut: " << std::to_string(hashCutoffs) << std::endl;
    }
};



using InfoCallback = std::function<void(const SearchInfo&)>;

#endif //ONYXCPP_TYPES_H