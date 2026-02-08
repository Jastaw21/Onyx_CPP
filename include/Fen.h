//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_FEN_H
#define ONYXCPP_FEN_H
#include <string>
#include <sstream>
#include "types.h"
#include "utils.h"


namespace FenHelpers {
    // clang-format off
    inline const std::string StartPos     = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    inline const std::string KiwiPeteFen  = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    inline const std::string Pos3Fen      = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
    inline const std::string Pos4Fen      = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    // clang-format on

    enum CastlingRights : uint8_t {
        WhiteKingside = 1 << 0,
        WhiteQueenside = 1 << 1,
        BlackKingside = 1 << 2,
        BlackQueenside = 1 << 3
    };

    struct FenDetails {
        Fen positionFen;
        bool whiteToMove;
        uint8_t castlingRights;
        Square enPassantSquare;
        int halfMoves;
        int fullMoves;
    };

    inline FenDetails ParseFen(const std::string& fenString){
        std::istringstream fenStream(fenString);
        FenDetails details;

        std::string position;
        std::string turnToMove;
        std::string castling;
        std::string enPassant;
        std::string half;
        std::string full;

        fenStream >> position >> turnToMove >> castling >> enPassant >> half >> full;

        details.positionFen = position;
        details.whiteToMove = turnToMove == "w";
        details.castlingRights = 0;
        if (castling.contains("K")) details.castlingRights |= WhiteKingside;
        if (castling.contains("Q")) details.castlingRights |= WhiteQueenside;
        if (castling.contains('k')) details.castlingRights |= BlackKingside;
        if (castling.contains('q')) details.castlingRights |= BlackQueenside;

        if (enPassant == "-")
            details.enPassantSquare = -1;
        else
            details.enPassantSquare = notationToSquare(enPassant);

        details.halfMoves = std::stoi(half);
        details.fullMoves = std::stoi(full);

        return details;
    }
}


#endif //ONYXCPP_FEN_H