//
// Created by jacks on 11/02/2026.
//

#include "Zobrist.h"

#include "Fen.h"
#include "utils.h"

// static initialisations
uint64_t Zobrist::whiteToMove;
uint64_t Zobrist::enPassantSquare[64];
uint64_t Zobrist::castlingRights[4];
uint64_t Zobrist::pieceHashes[12][64];
std::mt19937 Zobrist::rng;
bool Zobrist::initialized = false;

uint64_t Zobrist::fromBoard(Board* board){
    if (!initialized) init();

    uint64_t hash = 0ULL;
    for (int s = 0; s < 64; s++) {
        auto piece = board->getBoard()[s];
        if (!piece.exists()) continue;

        // get the value of this piece/square combo
        hash ^= pieceHashes[piece.index()][s];
    }

    if (board->whiteToMove())
        hash ^= whiteToMove;

    if (board->enPassantSquare() != -1) { hash ^= enPassantSquare[board->enPassantSquare()]; }

    if (board->castlingRights() != 0) {
        if (board->castlingRights() & FenHelpers::WhiteKingside) hash ^= castlingRights[0];
        if (board->castlingRights() & FenHelpers::WhiteQueenside) hash ^= castlingRights[1];
        if (board->castlingRights() & FenHelpers::BlackKingside) hash ^= castlingRights[2];
        if (board->castlingRights() & FenHelpers::BlackQueenside) hash ^= castlingRights[3];
    }

    return hash;
}

void Zobrist::applyMove(uint64_t& currentHash, const Move& move, const Piece pieceMoved,
                        const Piece captured = Piece(), const Square capturedOn = -1,
                        const uint8_t castlingPre = 0, const uint8_t castlingPost = 0,
                        const Square enPassantPre = -1, const Square enPassantPost = -1){
    // clang-format off
    if (!initialized) init();

    if (!pieceMoved.exists()) return;

    // swap turns
    currentHash ^= whiteToMove;

    // the piece has moved
    currentHash ^= pieceHashes[pieceMoved.index()][move.from()];
    currentHash ^= pieceHashes[pieceMoved.index()][move.to()];

    // bin off the captured piece
    if (captured.exists()) currentHash   ^= pieceHashes[captured.index()][capturedOn];

    if (move.isPromotion()) { const auto promotedPiece = Piece(move.promotionType(), pieceMoved.colour());
        currentHash ^= pieceHashes[pieceMoved.index()][move.to()]; // undo the pawn placement on the to square
        currentHash ^= pieceHashes[promotedPiece.index()][move.to()];
    }

    else if (move.flags() & Castling) {
        const auto toRaf = squareToRankAndFile(move.to());
        const auto rookMoved = pieceMoved.colour() == White ? Piece(Rook, White) : Piece(Rook, Black);
        const auto rookOriginFile = toRaf.file == 2 ? 0 : 7; // get the correct sided rook
        const auto rookOriginSquare = rankAndFileToSquare(toRaf.rank, rookOriginFile);

        const auto rookTargetFile = toRaf.file == 2 ? 3 : 5; // get the correct sided rook
        const auto rookTargetSquare = rankAndFileToSquare(toRaf.rank, rookTargetFile);

        currentHash ^= pieceHashes[rookMoved.index()][rookOriginSquare];
        currentHash ^= pieceHashes[rookMoved.index()][rookTargetSquare];
    }

    // action the enPassantSquare changes
    if (enPassantPre != -1) currentHash  ^= enPassantSquare[enPassantPre];
    if (enPassantPost != -1) currentHash ^= enPassantSquare[enPassantPost];

    // scout for any castling rights changes
    if (castlingPre != castlingPost) {
        if ((castlingPost & FenHelpers::WhiteKingside)  != (castlingPre & FenHelpers::WhiteKingside))  currentHash ^= castlingRights[0];
        if ((castlingPost & FenHelpers::WhiteQueenside) != (castlingPre & FenHelpers::WhiteQueenside)) currentHash ^= castlingRights[1];
        if ((castlingPost & FenHelpers::BlackKingside)  != (castlingPre & FenHelpers::BlackKingside))  currentHash ^= castlingRights[2];
        if ((castlingPost & FenHelpers::BlackQueenside) != (castlingPre & FenHelpers::BlackQueenside)) currentHash ^= castlingRights[3];
    }

    // clang-format on
}

void Zobrist::init(){
    rng.seed(123123123); // stable seed for now
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    whiteToMove = rng.operator()();

    fillRandomArray<uint64_t, 64>(&enPassantSquare[0], rng);
    fillRandomArray<uint64_t, 4>(&castlingRights[0], rng);

    for (int i = 0; i < 12; i++) { fillRandomArray<uint64_t, 64>(&pieceHashes[i][0], rng); }
    initialized = true;
}