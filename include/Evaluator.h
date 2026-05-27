//
// Created by jacks on 08/02/2026.
//

#ifndef ONYXCPP_EVALUATOR_H
#define ONYXCPP_EVALUATOR_H
#include "Board.h"
#include "types.h"

struct MaterialEval {
    int pieceSquareScore;
    int materialScore;
};

struct Material {
    Bitboard whitePawns = 0ULL;
    Bitboard blackPawns = 0ULL;

    Bitboard whiteKing = 0ULL;
    Bitboard blackKing = 0ULL;

    Bitboard whiteBishop = 0ULL;
    Bitboard blackBishop = 0ULL;

    Bitboard whiteRook = 0ULL;
    Bitboard blackRook = 0ULL;

    Bitboard whiteKnight = 0ULL;
    Bitboard blackKnight = 0ULL;

    Bitboard whiteQueen = 0ULL;
    Bitboard blackQueen = 0ULL;
};


class Evaluator {
public:

    static int Evaluate(const Board& board);
    static MaterialEval EvaluateMaterial(const Board& board, bool forWhite, float endGameRatio, Material& outMaterial);
    static int KingSafetyScore(bool forWhite, const Board& board, const Material& material);
    static int KingShieldScoreByColour(bool forWhite, const Board& board);
    static int KingOpenFileScore(bool forWhite, const Board& board, const Material& material);
    static int kingShieldPenalty;
    static int openfilePenalty;

private:

    static Psq pawnTables;
    static Psq rookTables;
    static Psq kingTables;
    static Psq queenTables;
    static Psq bishopTables;
    static Psq knightTables;

    static int getScoreOnSquare(PieceType type, Square onSquare, bool isWhite, bool endGame);
    static Psq& getTableByPieceType(const PieceType type);
    static std::array<Piece, 6> whitePieces;
    static std::array<Piece, 6> blackPieces;
};


#endif //ONYXCPP_EVALUATOR_H