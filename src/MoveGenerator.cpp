//
// Created by jacks on 06/02/2026.
//

#include "MoveGenerator.h"

#include "Fen.h"
#include "MagicBitboards.h"
#include "MoveList.h"
#include "Referee.h"

std::array<Piece,6> MoveGenerator::whitePieces = {
    Piece(Pawn,White),  Piece(Knight,White),Piece(King,White),Piece(Queen,White),Piece(Rook,White),Piece(Bishop,White)
};
 std::array<Piece,6>  MoveGenerator::blackPieces = {
    Piece(Pawn,Black),  Piece(Knight,Black),Piece(King,Black),Piece(Queen,Black),Piece(Rook,Black),Piece(Bishop,Black)
};

PromotionMode MoveGenerator::promotionMode = All;

void MoveGenerator::GenerateMoves(const Board& board, MoveList& moveSpan, bool capturesOnly){

    bool whiteToMove = board.whiteToMove();
    const Bitboard us = board.getOccupancy(whiteToMove ? White : Black);
    const Bitboard them = board.getOccupancy(whiteToMove ? Black : White);
    const Piece theirKingPiece = whiteToMove ? Piece(King, Black) : Piece(King,White);
    const Bitboard theirKing = board.getBoardByPiece(theirKingPiece);
    generateBasicMoves(board, moveSpan, us, them, theirKing, capturesOnly);
    generatePawnMoves(board,moveSpan,us, them, theirKing, capturesOnly);
    if (board.castlingRights() == 0  || capturesOnly) return;
    addCastlingmoves(board,moveSpan,us, them);
}


void MoveGenerator::generateBasicMoves(const Board& board, MoveList& moveSpan, const Bitboard us, const Bitboard them, const Bitboard theirKing, bool
                                       capturesOnly){

    for (const auto piece: board.whiteToMove() ? whitePieces : blackPieces) {
        if (piece.type() == Pawn) continue;

        Bitboard startingPoint = board.getBoardByPiece(piece);
        while (startingPoint) {
            const auto from = std::countr_zero(startingPoint);
            Bitboard movesFromHere = MagicBitboards::getMoves(piece, from, us | them);
            movesFromHere &= ~us; // can't go to own squares
            movesFromHere &= ~theirKing; // can't attack their king

            if (capturesOnly) movesFromHere &= them;

            // turn the move bitboard into actual moves
            while (movesFromHere) {
                const auto toSquare = static_cast<Square>(std::countr_zero(movesFromHere));
                const auto move = Move(from, toSquare, 0);

                moveSpan.add(move);

                movesFromHere &= movesFromHere - 1;
            }

            startingPoint &= startingPoint - 1;
        }
    }
}

void MoveGenerator::generatePawnMoves(const Board& board, MoveList& moveSpan, const Bitboard us, Bitboard them, const Bitboard theirKing, bool capturesOnly){
    const bool isWhite = board.whiteToMove();
    const auto pawn = isWhite ? Piece(Pawn, White) : Piece(Pawn, Black);

    Bitboard startingPoint = board.getBoardByPiece(pawn);
    while (startingPoint) {
        const auto from = std::countr_zero(startingPoint);

        Bitboard pushesFromHere = MagicBitboards::getPawnPushes(from,us | them, isWhite);
        pushesFromHere &= ~us; // can't go to own squares

        Bitboard attacks = MagicBitboards::getPawnAttacks(from, us | them, isWhite);
        Bitboard enPassantSquare = 0ULL;
        if (board.enPassantSquare() != -1) { enPassantSquare = 1ULL << board.enPassantSquare(); }
        them |= enPassantSquare;
        attacks &= them; // obviously, attacks can only happen on opponent squares or the ep square
        attacks &= ~theirKing; // cant attack their king
        pushesFromHere |= attacks;

        if (capturesOnly) pushesFromHere &= them;

        // turn the move bitboard into actual moves
        while (pushesFromHere) {
            const auto toSquare = static_cast<Square>(std::countr_zero(pushesFromHere));

            const bool isPromotion = (isWhite && toSquare >= 56) || (!isWhite && toSquare <= 7);
            const bool isEnPassant = toSquare == board.enPassantSquare();
            uint8_t flags = 0;
            if (isEnPassant) flags |= EnPassant;

            if (!isPromotion) {
                const auto move = Move(from, toSquare, flags);
                moveSpan.add(move);
            } else {
                const auto queening = Move(from,toSquare,PromotionQueen);
                const auto knighting = Move(from,toSquare,PromotionKnight);
                if (promotionMode == All) {
                    const auto rooking = Move(from,toSquare,PromotionRook);
                    const auto bishoping = Move(from,toSquare,PromotionBishop);
                    moveSpan.add(rooking);
                    moveSpan.add(bishoping);
                }
                moveSpan.add(queening);
                moveSpan.add(knighting);
            }

            pushesFromHere &= pushesFromHere - 1;
        }

        startingPoint &= startingPoint - 1;
    }
}

void MoveGenerator::addCastlingmoves(const Board& board, MoveList& moveSpan, Bitboard us, Bitboard them){
    bool whiteToMove = board.whiteToMove();
    // no castling rights remaining
    const auto ourKingsideRights = whiteToMove ? FenHelpers::CastlingRights::WhiteKingside : FenHelpers::CastlingRights::BlackKingside;
    const auto ourQueensideRights= whiteToMove ? FenHelpers::CastlingRights::WhiteQueenside : FenHelpers::CastlingRights::BlackQueenside;
    const auto ourCastlingRights = whiteToMove
                                       ? FenHelpers::CastlingRights::WhiteKingside |
                                         FenHelpers::CastlingRights::WhiteQueenside
                                       : FenHelpers::CastlingRights::BlackKingside |
                                         FenHelpers::CastlingRights::BlackQueenside;
    uint8_t castlingRights = board.castlingRights();
    if ((castlingRights & ourCastlingRights) == 0) return;


    auto qsMask = getCastlingMask(whiteToMove, true);
    auto ksMask = getCastlingMask(whiteToMove, false);

    auto ksClear = ((us | them) & ksMask) == 0;
    auto qsClear = ((us | them) & qsMask) == 0;

    Square kingSquare = whiteToMove ? 4 : 60;

    if (ksClear && (castlingRights & ourKingsideRights)) {
        Square targetSquare = whiteToMove ? 6 : 62;
        bool ksNotAttacked = true;
        Bitboard squaresToCheckForAttack = ksMask | (1ULL << kingSquare);
        while (squaresToCheckForAttack) {
            Square thisSquare = static_cast<Square>(std::countr_zero(squaresToCheckForAttack));
            if (Referee::SquareAttacked(thisSquare, board, !whiteToMove)) { ksNotAttacked = false; }
            squaresToCheckForAttack &= squaresToCheckForAttack - 1;
        }

        // clear, and no squares attacked - make a castling move
        if (ksNotAttacked) {
            uint8_t flags = Castling;
            auto castlingMove = Move(kingSquare, targetSquare, flags);
            moveSpan.add(castlingMove);
        }
    }

    if (qsClear && (castlingRights & ourQueensideRights)) {
        Square targetSquare = whiteToMove ? 2 : 58;
        bool qsNotAttacked = true;
        Bitboard squaresToCheckForAttack = qsMask | (1ULL << kingSquare);
        squaresToCheckForAttack &= ~ (1ULL << 1 | 1ULL << 57); // ignore b1/b8 being attacked
        while (squaresToCheckForAttack) {
            Square thisSquare = static_cast<Square>(std::countr_zero(squaresToCheckForAttack));
            if (Referee::SquareAttacked(thisSquare, board, !whiteToMove)) { qsNotAttacked = false; }
            squaresToCheckForAttack &= squaresToCheckForAttack - 1;
        }

        // clear, and no squares attacked - make a castling move
        if (qsNotAttacked) {
            uint8_t flags = Castling;
            auto castlingMove = Move(kingSquare, targetSquare, flags);
            moveSpan.add(castlingMove);
        }
    }
}