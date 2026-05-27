//
// Created by jacks on 07/02/2026.
//

#include "Referee.h"

#include "MagicBitboards.h"

bool Referee::MoveIsLegal(Board& board, const Move move){
    const auto pieceMoved = board.pieceAtSquare(move.from());

    if (!pieceMoved.exists()) return false; // can't be legal - isn't a piece there
    if (pieceMoved.colour() != (board.whiteToMove() ? White : Black)) return false; // moving out of turn

    if (pieceMoved.type() == King) { return fullLegalityTest(board, move); }

    const bool isWhite = pieceMoved.isWhite();
    const Colour colourMoved = pieceMoved.colour();

    const auto ourKingPiece = Piece(King, colourMoved);
    const Bitboard ourKingBoard = board.getOccupancy(ourKingPiece);
    const auto ourKingSquare = static_cast<Square>(std::countr_zero(ourKingBoard));
    Bitboard allOccupancy;

    // if we're not in check - only need to check for releasing pins
    if (!SquareAttacked(ourKingSquare, board, !isWhite, allOccupancy)) {

        // if it DOESNT release a pin, it must be legal
        return !wouldReleasePin(move.from(), move.to(), ourKingSquare, isWhite, board, allOccupancy);
    }
    // otherwise - we were in check, need to do full test
    return fullLegalityTest(board, move);
}

bool Referee::IsInCheck(const Board& board, const bool forWhite){
    const Piece relevantKing = forWhite ? Piece(King, White) : Piece(King, Black);
    const Bitboard kingBoard = board.getOccupancy(relevantKing);
    const auto kingSquare = static_cast<Square>(std::countr_zero(kingBoard));
    Bitboard outOccupancy;
    return SquareAttacked(kingSquare, board, !forWhite, outOccupancy);
}

bool Referee::SquareAttacked(const Square square, const Board& board, const bool byWhite, Bitboard& outOccupancy){
    const auto [rank, file] = squareToRankAndFile(square);
    const Colour colour = byWhite ? White : Black;

    // see if pawns attack this square
    const auto relevantPawn = Piece(Pawn, colour);
    const Bitboard pawnPositions = board.getOccupancy(relevantPawn);

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

    // try knights
    const Bitboard knightPlacements = board.getOccupancy(Piece(Knight, colour));
    outOccupancy = board.getOccupancy();
    const auto KnightsAttacksFromHere =
            MagicBitboards::getMoves(Piece(Knight, White), square, outOccupancy);
    if (knightPlacements & KnightsAttacksFromHere)
        return true;

    // try kings
    const Bitboard kingPlacements = board.getOccupancy(Piece(King, colour));
    const auto kingAttacksFromHere =
            MagicBitboards::getMoves(Piece(King, White), square, outOccupancy);
    if (kingPlacements & kingAttacksFromHere)
        return true;

    const Bitboard queens = board.getOccupancy(Piece(Queen, colour));
    const Bitboard diagonalThreats = queens | board.getOccupancy(Piece(Bishop, colour));

    // try diagonal threats
    if (diagonalThreats) {
        const auto diagonalAttacks =
                MagicBitboards::getMoves(Piece(Bishop, White), square, outOccupancy);
        if (diagonalThreats & diagonalAttacks)
            return true;
    }

    if (const Bitboard straightThreats = queens | board.getOccupancy(Piece(Rook, colour))) {
        const auto straightAttacks = MagicBitboards::getMoves(Piece(Rook, White), square, outOccupancy);
        if (straightAttacks & straightThreats)
            return true;
    }
    return false;
}

bool Referee::isRepetition(Board& board){
    if (board.History().size() < 2) return false;
    const auto currentHash = board.getHash();
    const auto historyEntries = board.History().size();
    const auto firstToSearch = historyEntries - 1;
    const int hmCutoff = static_cast<int>(board.History().size()) - board.halfMoves();
    const int startIndex = static_cast<int>(firstToSearch);

    for (auto i = startIndex; i >= hmCutoff && i >= 0; i--) {
        const auto previousHash = board.History()[i].hash;
        if (currentHash == previousHash) return true;
    }
    return false;
}

bool Referee::isCapture(const Board& board, const Move move){
    if (move.flags() & MoveFlags::EnPassant)
        return true;
    return board.pieceAtSquare(move.to()).exists();
}

bool Referee::fullLegalityTest(Board& board, const Move move){
    board.makeMove(move);
    const bool result = IsInCheck(board, !board.whiteToMove());
    board.unmakeMove(move);

    return !result;
}

bool Referee::wouldReleasePin(const Square pinnedFrom, const Square pinnedTo, const Square kingSquare,
                              const bool isWhite, const Board& board, const Bitboard allOccupancy){
    const auto ray = rayBetween(kingSquare, pinnedFrom);
    if (ray == 0) return false; // not on a ray, can't be pinned

    const Bitboard occupancyWithoutPotentialPinned = allOccupancy & ~(1ULL << pinnedFrom);

    const Colour theirColour = isWhite ? Black : White;

    const auto theirBishop = Piece(Bishop, theirColour);
    const auto theirQueen = Piece(Queen, theirColour);

    const Bitboard theirQueenPosition = board.getOccupancy(theirQueen);
    const Bitboard diagonalAttackers = board.getOccupancy(theirBishop) | theirQueenPosition;
    const Bitboard diagonalAttacks = MagicBitboards::getMoves(Piece(Bishop, White), kingSquare,
                                                              occupancyWithoutPotentialPinned);

    // there is a queen or bishop on a square that can attack the king
    if (const Bitboard diagonalThreats = diagonalAttackers & diagonalAttacks) {
        const auto asSquare = static_cast<Square>(std::countr_zero(diagonalThreats));

        // if we're moving on that ray, then it's fine, if not illegal
        const auto diagRay = rayBetween(asSquare, kingSquare);
        return (diagRay & 1ULL << pinnedTo) == 0;
    }

    const auto relevantRook = Piece(Rook, theirColour);
    const Bitboard straightAttackers = board.getOccupancy(relevantRook) | theirQueenPosition;
    const Bitboard straightAttacks = MagicBitboards::getMoves(Piece(Rook, White), kingSquare,
                                                              occupancyWithoutPotentialPinned);

    if (const Bitboard straightThreats = straightAttackers & straightAttacks) {
        const auto asSquare = static_cast<Square>(std::countr_zero(straightThreats));

        // if we're moving on that ray, then it's fine, if not illegal
        const auto diagRay = rayBetween(asSquare, kingSquare);
        return (diagRay & 1ULL << pinnedTo) == 0;
    }
    return false;
}