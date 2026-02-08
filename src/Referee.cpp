//
// Created by jacks on 07/02/2026.
//

#include "../include/Referee.h"

#include "../include/MagicBitboards.h"
bool Referee::wouldReleasePin(const Square pinnedFrom, const Square pinnedTo, const Square kingSquare, const bool isWhite, Board& board){

    const auto ray = rayBetween(kingSquare,pinnedFrom);
    if (ray == 0) return false; // not on a ray, can't be pinned

    const Bitboard occupancyWithoutPotentialPinned = board.getOccupancy() & ~(1ULL << pinnedFrom);

    const auto theirBishop = Piece(Bishop, isWhite ? Black : White );
    const auto theirQueen = Piece(Queen, isWhite ? Black : White );

    const Bitboard theirQueenPosition = board.getBoardByPiece(theirQueen);
    const Bitboard diagonalAttackers = board.getBoardByPiece(theirBishop) | theirQueenPosition;
    const Bitboard diagonalAttacks = MagicBitboards::getMoves(Piece(Bishop,White),kingSquare,occupancyWithoutPotentialPinned);
    const Bitboard diagonalThreats = diagonalAttackers & diagonalAttacks;

    // there is a queen or bishop on a square that can attack the king
    if (diagonalThreats) {
        const auto asSquare = static_cast<Square>(std::countr_zero(diagonalThreats));

        // if we're moving on that ray then it's fine, if not illegal
        const auto ray = rayBetween(asSquare,kingSquare);
        return (ray & (1ULL << pinnedTo)) == 0;
    }

    const auto relevantRook = Piece(Rook, isWhite ? Black : White );
    const Bitboard straightAttackers = board.getBoardByPiece(relevantRook) | theirQueenPosition;
    const Bitboard straightAttacks = MagicBitboards::getMoves(Piece(Rook,White),kingSquare,occupancyWithoutPotentialPinned);
    const Bitboard straightThreats = straightAttackers & straightAttacks;

    if (straightThreats) {
        const auto asSquare = static_cast<Square>(std::countr_zero(straightThreats));

        // if we're moving on that ray then it's fine, if not illegal
        const auto ray = rayBetween(asSquare,kingSquare);
        return (ray & (1ULL << pinnedTo)) == 0;
    }
return false;

}

bool Referee::MoveIsLegal(Board& board, const Move move){
    const auto pieceMoved = board.pieceAtSquare(move.from());

    if (pieceMoved.type() == King) { return fullLegalityCheck(board, move); }

    const bool isWhite = pieceMoved.isWhite();
    if (const bool inCheck = IsInCheck(board, isWhite); !inCheck) {

        const auto relevantKing = isWhite ? Piece(King, White) : Piece(King, Black);
        const Bitboard kingBoard = board.getBoardByPiece(relevantKing);
        const auto kingSquare = static_cast<Square>(std::countr_zero(kingBoard));

        return !wouldReleasePin(move.from(), move.to(), kingSquare, isWhite, board);
    }
    return fullLegalityCheck(board, move);
}


bool Referee::IsInCheck(Board& board, bool forWhite){

    const Piece relevantKing = forWhite ? Piece(King, White) : Piece(King,Black);
    const Bitboard kingBoard= board.getBoardByPiece(relevantKing);
    const auto kingSquare = static_cast<Square>(std::countr_zero(kingBoard));

    return SquareAttacked(kingSquare,board, !forWhite);

}
bool Referee::SquareAttacked(const Square square, const Board& board, const bool byWhite){

    const auto [rank, file] = squareToRankAndFile(square);

    // see if pawns attack this square
    const auto relevantPawn = byWhite ? Piece(Pawn, White) : Piece(Pawn, Black);
    const Bitboard pawnPositions = board.getBoardByPiece(relevantPawn);

    // can be attacked from the left
    if (file > 0) {
        const Square targetSquare = square + (byWhite ? -9 : 7);
        if (targetSquare >= 0 && targetSquare <= 63)
            if (1ULL << targetSquare & pawnPositions) return true;
    }
    if (file < 7) {
        const Square targetSquare = square + (byWhite ? -7 : 9);
        if (targetSquare >= 0 && targetSquare <= 63)
            if (1ULL << targetSquare & pawnPositions) return true;
    }
    const Bitboard thisSquare = 1ULL << square;

    // try knights
    const Bitboard knightPlacements = board.getBoardByPiece(byWhite ? Piece(Knight, White) : Piece(Knight, Black));
    const Bitboard occupancy = board.getOccupancy();
    const auto KnightsAttacksFromHere =
            MagicBitboards::getMoves(Piece(Knight, White), square, occupancy);
    if (knightPlacements & KnightsAttacksFromHere)
        return true;

    // try kings
    const Bitboard kingPlacements = board.getBoardByPiece(byWhite ? Piece(King, White) : Piece(King, Black));
    const auto kingAttacksFromHere =
            MagicBitboards::getMoves(Piece(King, White), square, occupancy);
    if (kingPlacements & kingAttacksFromHere)
        return true;

    const Bitboard queens = board.getBoardByPiece(byWhite ? Piece(Queen, White) : Piece(Queen, Black));
    const Bitboard diagonalThreats = queens | board.getBoardByPiece(byWhite ? Piece(Bishop, White) : Piece(Bishop, Black));

    // try diagonal threats
    if (diagonalThreats) {
        const auto diagonalAttacks =
                MagicBitboards::getMoves(Piece(Bishop, White), square, occupancy);
        if (diagonalThreats & diagonalAttacks)
            return true;
    }

    const Bitboard straightThreats = queens |board.getBoardByPiece(byWhite ? Piece(Rook, White) : Piece(Rook, Black));
    if (straightThreats) {
        const auto straightAttacks = MagicBitboards::getMoves(Piece(Rook, White), square, occupancy);
        if (straightAttacks & straightThreats)
            return true;
    }
    return false;
}

bool Referee::fullLegalityCheck(Board& board, const Move move){
    board.makeMove(move);
    const bool result = IsInCheck(board, !board.whiteToMove());
    board.unmakeMove(move);

    return !result;
}