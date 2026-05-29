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
    Bitboard Pawns = 0ULL;
    int pawnCount = 0;

    Bitboard King = 0ULL;
    int kingCount = 0;

    Bitboard Bishop = 0ULL;
    int bishopCount = 0;

    Bitboard Rook = 0ULL;
    int rookCount = 0;

    Bitboard Knight = 0ULL;
    int knightCount = 1; // always 1

    Bitboard Queen = 0ULL;
    int queenCount = 0;
};



class Evaluator {
public:

    static int Evaluate(const Board& board);
    static MaterialEval EvaluateMaterial(const Board& board, bool forWhite, Material& outMaterial);
    static int KingSafetyScore(bool forWhite, const Board& board, const Material& material);
    static int KingShieldScoreByColour(bool forWhite, const Board& board);
    static int KingOpenFileScore(bool forWhite, const Board& board, const Material& material);
    static int PassedPawnScore(bool forWhite, const Board& board, const Material& whiteMaterial, const Material& blackMaterial);
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