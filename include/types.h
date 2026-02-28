//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_TYPES_H
#define ONYXCPP_TYPES_H
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <cmath>
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

using Psq = std::array<sp, 64>;


struct Statistics {
    int depth = 0;
    uint64_t nodes = 0;
    uint64_t qNodes = 0;
    uint64_t betaCutoffs = 0;
    int hashCutoffs = 0;

    int emptyTTMoves = 0;
    int totalTTMoves = 0;

    float ebf() const{ return depth > 0 ? std::pow(nodes, 1.0f / depth) : 0.0f; }

    void Log() const{
        std::cerr << "ENGINE\n Nodes: " << std::to_string(nodes) << " QNodes: " << std::to_string(qNodes) << " ebf : "
                << std::to_string(ebf()) << " betaCutoffs: " << std::to_string(betaCutoffs) << " hash cut: " <<
                std::to_string(hashCutoffs) << " empty TT " << std::to_string(emptyTTMoves) << " total tt " << std::to_string(totalTTMoves) <<  std::endl;
    }
};


using InfoCallback = std::function<void(const SearchInfo&)>;

#endif //ONYXCPP_TYPES_H