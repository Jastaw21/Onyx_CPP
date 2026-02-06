//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_UTILS_H
#define ONYXCPP_UTILS_H

#include <locale>
#include "Piece.h"

struct RankAndFile {
    int rank;
    int file;
};

inline PieceType charToPieceType(const char c){
    switch (std::tolower((c))) {
        case 'p': return Pawn;
        case 'n': return Knight;
        case 'r': return Rook;
        case 'q': return Queen;
        case 'k': return King;
        case 'b': return Bishop;
    }

    return PieceType::NoType;
}

inline char pieceTypeToChar(const PieceType type){
    switch (type) {
        case Pawn: return 'p';
        case Knight: return 'n';
        case Rook: return 'r';
        case Queen: return 'q';
        case King: return 'k';
        case Bishop: return 'b';
    }
    return 'X';
}

inline RankAndFile squareToRankAndFile(const Square square){
    const int file = square % 8;
    const int rank = square / 8;

    return RankAndFile{rank, file};
}

inline Square rankAndFileToSquare(const int rank, const int file){ return rank * 8 + file; }

inline Square notationToSquare(const std::string& notation){
    auto file = notation[0];
    auto rank = notation[1];

    auto fileIndex = file - 'a';
    auto rankIndex = rank - '1';
    return rankAndFileToSquare(rankIndex, fileIndex);
}

inline std::string squareToNotation(const Square square){
    auto rankAndFile = squareToRankAndFile(square);

    std::string notation;
    char fileChar = 'a' + rankAndFile.file;
    auto rankChar = '1' + rankAndFile.rank;
    notation += fileChar;
    notation += rankChar;
    return notation;
}


#endif //ONYXCPP_UTILS_H