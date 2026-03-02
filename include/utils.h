//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_UTILS_H
#define ONYXCPP_UTILS_H

#include <locale>
#include <random>

#include "Move.h"
#include "Piece.h"
#include "types.h"

struct RankAndFile {
    int rank;
    int file;
};

inline PieceType charToPieceType(const char c){
    switch (std::tolower(c)) {
        case 'p': return Pawn;
        case 'n': return Knight;
        case 'r': return Rook;
        case 'q': return Queen;
        case 'k': return King;
        case 'b': return Bishop;
            default:return NoType;
    }
}

inline char pieceTypeToChar(const PieceType type){
    switch (type) {
        case Pawn: return 'p';
        case Knight: return 'n';
        case Rook: return 'r';
        case Queen: return 'q';
        case King: return 'k';
        case Bishop: return 'b';
        default: throw std::runtime_error("Tried to get character of unknown type");
    }

}

inline RankAndFile squareToRankAndFile(const Square square){
    const int file = square % 8;
    const int rank = square / 8;

    return RankAndFile{rank, file};
}

inline Square rankAndFileToSquare(const int rank, const int file){ return rank * 8 + file; }

inline Square notationToSquare(const std::string& notation){
    const auto file = notation[0];
    const auto rank = notation[1];

    const auto fileIndex = file - 'a';
    const auto rankIndex = rank - '1';
    return rankAndFileToSquare(rankIndex, fileIndex);
}

inline std::string squareToNotation(const Square square){
    const auto rankAndFile = squareToRankAndFile(square);

    std::string notation;
    const char fileChar = 'a' + rankAndFile.file;
    const auto rankChar = '1' + rankAndFile.rank;
    notation += fileChar;
    notation += rankChar;
    return notation;
}

inline Bitboard rayBetween(const Square a, const Square b){

    Bitboard ray = 0ULL;
    const RankAndFile rafA = squareToRankAndFile(a);
    const RankAndFile rafB = squareToRankAndFile(b);

    const bool isDiag = std::abs(rafA.rank - rafB.rank) == std::abs(rafA.file - rafB.file);
    const bool isStraight = rafA.file == rafB.file || rafA.rank == rafB.rank;

    if (!isDiag && ! isStraight) return ray;

    const auto deltaRank = rafB.rank - rafA.rank;
    const auto deltaFile = rafB.file - rafA.file;

    int stepsInRay;
    if (isDiag) stepsInRay = deltaRank;
    else {stepsInRay = deltaRank == 0 ? deltaFile : deltaRank;}

    int rankSign, fileSign;
    if (deltaRank < 0) rankSign =-1;
    else if (deltaRank == 0) rankSign =0;
    else rankSign = 1;

    if (deltaFile < 0) fileSign =-1;
    else if (deltaFile == 0) fileSign =0;
    else fileSign = 1;

    for (int step = 0; step <= std::abs(stepsInRay); step++) { const auto newRank = rafA.rank + step * rankSign;
        const auto newFile = rafA.file + step * fileSign;
        const auto newSquare = rankAndFileToSquare(newRank,newFile);
        ray |= 1ULL << newSquare;
    }
    return ray;
}

inline Move moveFromNotation(const std::string& notation){
    const auto from = notation.substr(0,2);
    const auto to = notation.substr(2,2);
    const auto fromSquare = notationToSquare(from);
    const auto toSquare = notationToSquare(to);
    uint8_t flags = 0;

    if (notation.size() == 5) { const char promotionChar = notation[4];
        const auto promotionType = charToPieceType(promotionChar);
        if (promotionType == Queen ) flags |= PromotionQueen;
        if (promotionType == Rook ) flags |= PromotionRook;
        if (promotionType == Bishop ) flags |= PromotionBishop;
        if (promotionType == Knight ) flags |= PromotionKnight;
    }
    return Move(fromSquare,toSquare,flags);
}

inline std::string moveToNotation(const Move move) {
    std::string notation;
    notation += squareToNotation(move.from());
    notation += squareToNotation(move.to());
    if (move.isPromotion()) notation += pieceTypeToChar(move.promotionType());
    return notation;
}

constexpr Bitboard getCastlingMask(const bool forWhite, const bool forQueenside){
    if (forWhite) return forQueenside ? 0xe : 0x60;
    return forQueenside ?0xe00000000000000 :0x6000000000000000;
}

template<typename ArrayType, std::size_t FirstDim, std::size_t SecondDim = 0>
void fillRandomArray(ArrayType* array, std::mt19937& rng) {
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

    if constexpr (SecondDim == 0) {
        // 1D array
        for (std::size_t i = 0; i < FirstDim; i++) {
            array[i] = dist(rng);
        }
    } else {
        // 2D array
        for (std::size_t i = 0; i < FirstDim; i++) {
            for (std::size_t j = 0; j < SecondDim; j++) {
                array[i][j] = dist(rng);
            }
        }
    }
}

constexpr int MATE = 20000;
constexpr int INF = 30000;
inline bool isMateScore(const int score){ return std::abs(score) >= MATE; }
inline int correctedMatedScore(const int score, const int plyFromRoot){
    if (!isMateScore(score)) return score;
    if (score > 0) return score - plyFromRoot;
    return score + plyFromRoot;
}

#endif //ONYXCPP_UTILS_H