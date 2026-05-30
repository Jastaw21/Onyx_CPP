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

struct MaterialRecord {
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

    // main entry point, combines all evaluations together
    static int Evaluate(const Board& board);


    // individual scoring functions
    static int MaterialScore(const MaterialRecord& relevantMaterial);
    static int PieceSquareScore(bool forWhite, const MaterialRecord& relevantMaterial, float endGameScore);
    static int KingShieldScoreByColour(bool forWhite, const Board& board);
    static int KingOpenFileScore(bool forWhite, const Board& board, const MaterialRecord& material);
    static int PassedPawnScore(bool forWhite, const MaterialRecord& whiteMaterial, const MaterialRecord& blackMaterial);

    // tweakable parameters
    static int kingShieldPenalty;
    static int openfilePenalty;

    // precaches the material record of occupancy by piece, for one colour at a time
    static void PopulateMaterialRecord(const Board& board, bool forWhite, MaterialRecord& outMaterial);

private:

    // piece square score tables
    static Psq pawnTables;
    static Psq rookTables;
    static Psq kingTables;
    static Psq queenTables;
    static Psq bishopTables;
    static Psq knightTables;

    // helpers to clean up psq function
    static int getScoreOnSquare(PieceType type, Square onSquare, bool isWhite, bool endGame);
    static Psq& getTableByPieceType(const PieceType type);
    static std::array<Piece, 6> whitePieces;
    static std::array<Piece, 6> blackPieces;
};

float actualPieceValue(const MaterialRecord& material);

#endif //ONYXCPP_EVALUATOR_H