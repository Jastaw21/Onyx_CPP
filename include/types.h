//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_TYPES_H
#define ONYXCPP_TYPES_H
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <variant>
struct SearchInfo;
using Square = int8_t;
using Bitboard = uint64_t;
using Fen = std::string;
using ZobristHash = uint64_t;

// score pair and piecesquaretable
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

    int reducedSearches = 0;
    int fullResearches = 0;


    float ebf() const{ return depth > 0 ? std::pow(nodes, 1.0f / depth) : 0.0f; }

    void Log() const{
        if (false)
        std::cerr << "ENGINE\n Nodes: " << std::to_string(nodes) << " QNodes: " << std::to_string(qNodes) << " ebf : "
                << std::to_string(ebf()) << " betaCutoffs: " << std::to_string(betaCutoffs) << " hash cut: " <<
                std::to_string(hashCutoffs) << " reduced: " << std::to_string(reducedSearches) << " full: " << std::to_string(fullResearches) <<  std::endl;
    }
};
using OptionValue = std::variant<int, bool, std::string>;
using InfoCallback = std::function<void(const SearchInfo&)>;


constexpr inline uint64_t FILE_A = 0x101010101010101;
constexpr inline uint64_t FILE_B = 0x202020202020202;
constexpr inline uint64_t FILE_C = 0x404040404040404;
constexpr inline uint64_t FILE_D = 0x808080808080808;
constexpr inline uint64_t FILE_E = 0x1010101010101010;
constexpr inline uint64_t FILE_F = 0x2020202020202020;
constexpr inline uint64_t FILE_G = 0x4040404040404040;
constexpr inline uint64_t FILE_H = 0x8080808080808080;

constexpr uint64_t fileMask(const int fileIndex){
    switch (fileIndex) {
        case (0): {
            return FILE_A;
            break;
        }
        case (1): {
            return FILE_B;
            break;
        }
        case (2): {
            return FILE_C;
            break;
        }
        case (3): {
            return FILE_D;
            break;
        }
        case (4): {
            return FILE_E;
            break;
        }
        case (5): {
            return FILE_F;
            break;
        }
        case (6): {
            return FILE_G;
            break;
        }
        case (7): {
            return FILE_H;
            break;
        }
        default: return 0ULL;
    }
}

#endif //ONYXCPP_TYPES_H