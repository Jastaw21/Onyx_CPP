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

class Evaluator {

public:
    static int Evaluate(Board& board);
    static MaterialEval EvaluateMaterial(Board& board, bool forWhite, float endGameRatio);

private:

    static Psq pawnTables;
    static Psq rookTables;
    static Psq kingTables;
    static Psq queenTables;
    static Psq bishopTables;
    static Psq knightTables;

    static int getScoreOnSquare(PieceType type, Square onSquare, bool isWhite, bool endGame);
    static  std::array<Piece,6> whitePieces ;
    static std::array<Piece,6>  blackPieces ;
    static  std::array<int,6> pieceValues;
};


#endif //ONYXCPP_EVALUATOR_H