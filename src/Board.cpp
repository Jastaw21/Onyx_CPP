//
// Created by jacks on 06/02/2026.
//

#include "../include/Board.h"
#include "../include/utils.h"

void Board::setOff(const Piece piece, const Square square){
    board_[square] = Piece();
    boards_[piece.index()] &= ~(1ull << square);
}

Board::Board(){
    board_ = std::array<Piece, 64>{Piece()};
    boards_ = std::array<Bitboard, 12>{0ull};
}

Board::Board(Fen fen){
    board_ = std::array<Piece, 64>{Piece()};
    boards_ = std::array<Bitboard, 12>{0ull};
    FromFen(fen);
}

void Board::loadFen(Fen fen){ FromFen(fen); }

Fen Board::getFen(){
    Fen builtFen = "";

    for (int rank = 7; rank >= 0; rank--) {
        int numberOfEmpties = 0;
        for (int file = 0; file <= 7; file++) {
            Square square = rankAndFileToSquare(rank, file);
            auto piece = board_[square];

            if (!piece.exists()) {
                numberOfEmpties++;
                if (file == 7) {
                    builtFen += std::to_string(numberOfEmpties);
                }
                continue;
            }

            if (numberOfEmpties > 0) {
                builtFen += std::to_string(numberOfEmpties);
                numberOfEmpties = 0;
            }

            auto charOfThisPiece = pieceTypeToChar(piece.type());
            if (piece.isWhite())
                charOfThisPiece = std::toupper(charOfThisPiece);
            builtFen += charOfThisPiece;
        }
        if (rank != 0)
            builtFen += '/';
    }

    return builtFen;
}

void Board::setOn(const Piece piece, const Square square){
    board_[square] = piece;
    boards_[piece.index()] |= 1ull << square;
}

void Board::move(const Piece piece, const Square from, const Square to){
    setOff(piece, from);
    setOn(piece, to);
}

void Board::FromFen(Fen fen){
    // reset the boards
    board_.fill(Piece());
    boards_.fill(0ull);

    auto fen_ = fen;

    int rank = 7;
    int file = 0;
    size_t i = 0;

    while (i < fen.size() && fen[i] != ' ') {
        const char c = fen[i];

        // go to end of line, do a "carriage return"
        if (c == '/') {
            rank--;
            file = 0;
        }
        // got a skip to next piece
        else if (isdigit(c)) { file += c - '0'; } else {
            const bool isWhite = isupper(c);
            const PieceType type = charToPieceType(c);
            const Piece thisPiece = Piece(type, isWhite ? White : Black);
            Square thisSquare = rankAndFileToSquare(rank,file);
            setOn(thisPiece, thisSquare);

            file++;
        }
        i++;
    }
}