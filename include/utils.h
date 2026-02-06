//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_UTILS_H
#define ONYXCPP_UTILS_H

#include <locale>
#include "Piece.h"


inline PieceType charToPieceType(char c){
    switch (std::tolower((c))) {
        case 'p': return Pawn;
        case 'n': return Knight;
        case 'r': return Rook;
        case 'q': return Queen;
        case 'k': return King;
        case 'b': return Bishop;
    }
}

inline char pieceTypeToChar(PieceType type){
    switch (type) {
        case Pawn: return 'p';
        case Knight: return 'n';
        case Rook: return 'r';
        case Queen : return 'q';
        case King : return 'k';
        case Bishop : return 'b';
    }
}

inline Square rankAndFileToSquare(int rank, int file){ return rank * 8 + file; }

#endif //ONYXCPP_UTILS_H