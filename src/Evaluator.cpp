//
// Created by jacks on 08/02/2026.
//

#include "../include/Evaluator.h"



// Pawn, Knight, Bishop, Rook, Queen, King, NoType
std::array<int,6> Evaluator::pieceValues = {100,300,320,500,900,0};

std::array<Piece,6> Evaluator::whitePieces = {
    Piece(Pawn,White),  Piece(Knight,White),Piece(King,White),Piece(Queen,White),Piece(Rook,White),Piece(Bishop,White)
};
std::array<Piece,6>  Evaluator::blackPieces = {
    Piece(Pawn,Black),  Piece(Knight,Black),Piece(King,Black),Piece(Queen,Black),Piece(Rook,Black),Piece(Bishop,Black)
};



// clang-format off
Psq Evaluator::pawnTables = Psq{
    sp{0,  0},   sp{0, 0},    sp{0, 0},    sp{0, 0},    sp{0, 0},    sp{0, 0},    sp{0, 0},    sp{0, 0},
    sp{50, 80},  sp{50,80},   sp{50,80},   sp{50,80},   sp{50, 80},  sp{50, 80},  sp{50, 80},  sp{50, 80},
    sp{10, 50},  sp{10, 50},  sp{20, 50},  sp{30, 50},  sp{30, 50},  sp{20, 50},  sp{10, 50},  sp{10, 50},
    sp{5,  30},  sp{5, 30},   sp{10, 30},  sp{25, 30},  sp{25, 30},  sp{10, 30},  sp{5, 30},   sp{5, 30},
    sp{0,  20},  sp{0, 20},   sp{0, 20},   sp{20, 20},  sp{20, 20},  sp{0, 20},   sp{0, 20},   sp{0, 20},
    sp{5,  10},  sp{-5, 10},  sp{-10, 10}, sp{0, 10},   sp{0, 10},   sp{-10, 10}, sp{-5, 10},  sp{5, 10},
    sp{5,  10},  sp{10, 10},  sp{10, 10},  sp{-20, 10}, sp{-20, 10}, sp{10, 10},  sp{10, 10},  sp{5, 10},
    sp{0,  0},   sp{0, 0},    sp{0, 0},    sp{0, 0},    sp{0, 0},    sp{0, 0},    sp{0, 0},    sp{0, 0}
};

Psq Evaluator::rookTables = Psq{
    sp{ 0,  5},  sp{ 0,  5},  sp{ 0,  5},  sp{ 0,  5},  sp{ 0,  5},  sp{ 0,  5},  sp{ 0,  5},  sp{ 0,  5},
    sp{ 5,  8},  sp{10, 10},  sp{10, 10},  sp{10, 10},  sp{10, 10},  sp{10, 10},  sp{10, 10},  sp{ 5,  8},
    sp{-5,  5},  sp{ 0,  5},  sp{ 0,  5},  sp{ 0,  5},  sp{ 0,  5},  sp{ 0,  5},  sp{ 0,  5},  sp{-5,  5},
    sp{-5, -2},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{-5, -2},
    sp{-5, -2},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{-5, -2},
    sp{-5, -2},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{ 0,  0},  sp{-5, -2},
    sp{-5,-12},  sp{ 0,-10},  sp{ 0,-10},  sp{ 0,-10},  sp{ 0,-10},  sp{ 0,-10},  sp{ 0,-10},  sp{-5,-12},
    sp{ 0,-12},  sp{ 0,-10},  sp{ 0,-10},  sp{ 5,-10},  sp{ 5,-10},  sp{ 0,-10},  sp{ 0,-10},  sp{ 0,-12}
};

Psq Evaluator::kingTables = Psq{
    sp{-80,-20}, sp{-70,-10}, sp{-70,-10}, sp{-70,-10}, sp{-70,-10}, sp{-70,-10}, sp{-70,-10}, sp{-80,-20},
    sp{-60, -5}, sp{-60,  0}, sp{-60,  5}, sp{-60,  5}, sp{-60,  5}, sp{-60,  5}, sp{-60,  0}, sp{-60, -5},
    sp{-40,-10}, sp{-50, -5}, sp{-50, 20}, sp{-60, 30}, sp{-60, 30}, sp{-50, 20}, sp{-50, -5}, sp{-40,-10},
    sp{-30,-15}, sp{-40,-10}, sp{-40, 35}, sp{-50, 45}, sp{-50, 45}, sp{-40, 35}, sp{-40,-10}, sp{-30,-15},
    sp{-20,-20}, sp{-30,-15}, sp{-30, 30}, sp{-40, 40}, sp{-40, 40}, sp{-30, 30}, sp{-30,-15}, sp{-20,-20},
    sp{-10,-25}, sp{-20,-20}, sp{-20, 20}, sp{-20, 25}, sp{-20, 25}, sp{-20, 20}, sp{-20,-20}, sp{-10,-25},
    sp{ 20,-30}, sp{ 20,-25}, sp{ -5,  0}, sp{ -5,  0}, sp{ -5,  0}, sp{ -5,  0}, sp{ 20,-25}, sp{ 20,-30},
    sp{ 20,-50}, sp{ 30,-30}, sp{ 10,-30}, sp{  0,-30}, sp{  0,-30}, sp{ 10,-30}, sp{ 30,-30}, sp{ 20,-50}
};

Psq Evaluator::queenTables = Psq{
    sp{-20,-20}, sp{-12,-12}, sp{-10,-10}, sp{ -5, -5}, sp{ -5, -5}, sp{-10,-10}, sp{-12,-12}, sp{-20,-20},
    sp{-10,-10}, sp{ -5, -5}, sp{  0,  0}, sp{  2,  2}, sp{  2,  2}, sp{  0,  0}, sp{ -5, -5}, sp{-10,-10},
    sp{-10,-10}, sp{  0,  0}, sp{  5,  5}, sp{  6,  6}, sp{  6,  6}, sp{  5,  5}, sp{  0,  0}, sp{-10,-10},
    sp{ -5, -5}, sp{  0,  0}, sp{  5,  5}, sp{  7,  7}, sp{  7,  7}, sp{  5,  5}, sp{  0,  0}, sp{ -5, -5},
    sp{ -5, -5}, sp{  0,  0}, sp{  5,  5}, sp{  7,  7}, sp{  7,  7}, sp{  5,  5}, sp{  0,  0}, sp{ -5, -5},
    sp{-10,-10}, sp{  5,  5}, sp{  5,  5}, sp{  5,  5}, sp{  5,  5}, sp{  5,  5}, sp{  0,  0}, sp{-10,-10},
    sp{-10,-10}, sp{  1,  1}, sp{  2,  2}, sp{  3,  3}, sp{  3,  3}, sp{  2,  2}, sp{  1,  1}, sp{-10,-10},
    sp{-20,-20}, sp{-12,-12}, sp{-10,-10}, sp{ -5, -5}, sp{ -5, -5}, sp{-10,-10}, sp{-12,-12}, sp{-20,-20}
};

Psq Evaluator::bishopTables = Psq{
    sp{-10,-10}, sp{-10,-10}, sp{-10,-10}, sp{-10,-10}, sp{-10,-10}, sp{-10,-10}, sp{-10,-10}, sp{-10,-10},
    sp{-10,-10}, sp{  0,  0}, sp{  0,  0}, sp{  0,  0}, sp{  0,  0}, sp{  0,  0}, sp{  0,  0}, sp{-10,-10},
    sp{-10,-10}, sp{  0,  0}, sp{ 15, 15}, sp{ 10, 10}, sp{ 10, 10}, sp{ 15, 15}, sp{  0,  0}, sp{-10,-10},
    sp{-10,-10}, sp{  5,  5}, sp{  5,  5}, sp{ 10, 10}, sp{ 10, 10}, sp{  5,  5}, sp{  5,  5}, sp{-10,-10},
    sp{-10,-10}, sp{  0,  0}, sp{ 10, 10}, sp{ 10, 10}, sp{ 10, 10}, sp{ 10, 10}, sp{  0,  0}, sp{-10,-10},
    sp{-10,-10}, sp{ 10, 10}, sp{ 10, 10}, sp{ 10, 10}, sp{ 10, 10}, sp{ 10, 10}, sp{ 10, 10}, sp{-10,-10},
    sp{-10,-10}, sp{ 15, 15}, sp{  0,  0}, sp{  0,  0}, sp{  0,  0}, sp{  0,  0}, sp{ 15, 15}, sp{-10,-10},
    sp{-10,-10}, sp{-10,-10}, sp{-10,-10}, sp{-10,-10}, sp{-10,-10}, sp{-10,-10}, sp{-10,-10}, sp{-10,-10}
};


Psq Evaluator::knightTables = Psq{
    sp{-50,-50}, sp{-40,-40}, sp{-30,-30}, sp{-30,-30}, sp{-30,-30}, sp{-30,-30}, sp{-40,-40}, sp{-50,-50},
    sp{-40,-40}, sp{-20,-20}, sp{  0,  0}, sp{  0,  0}, sp{  0,  0}, sp{  0,  0}, sp{-20,-20}, sp{-40,-40},
    sp{-30,-30}, sp{  0,  0}, sp{ 10, 10}, sp{ 15, 15}, sp{ 15, 15}, sp{ 10, 10}, sp{  0,  0}, sp{-30,-30},
    sp{-30,-30}, sp{  5,  5}, sp{ 15, 15}, sp{ 20, 20}, sp{ 20, 20}, sp{ 15, 15}, sp{  5,  5}, sp{-30,-30},
    sp{-30,-30}, sp{  0,  0}, sp{ 15, 15}, sp{ 20, 20}, sp{ 20, 20}, sp{ 15, 15}, sp{  0,  0}, sp{-30,-30},
    sp{-30,-30}, sp{  5,  5}, sp{ 10, 10}, sp{ 15, 15}, sp{ 15, 15}, sp{ 10, 10}, sp{  5,  5}, sp{-30,-30},
    sp{-40,-40}, sp{-20,-20}, sp{  0,  0}, sp{  5,  5}, sp{  5,  5}, sp{  0,  0}, sp{-20,-20}, sp{-40,-40},
    sp{-50,-50}, sp{-40,-40}, sp{-30,-30}, sp{-30,-30}, sp{-30,-30}, sp{-30,-30}, sp{-40,-40}, sp{-50,-50}
};


// clang-format on
int Evaluator::Evaluate(Board& board){
    const Bitboard whiteCount = std::popcount(board.getOccupancy(White));
    const Bitboard blackCount = std::popcount(board.getOccupancy(Black));

    const auto whitePsqScore = EvaluateMaterial(board,true,0);
    const auto blackPsqScore = EvaluateMaterial(board,false,0);

    int score = 0;

    score += whiteCount - blackCount;
    score += whitePsqScore.materialScore - blackPsqScore.materialScore;
    score += whitePsqScore.pieceSquareScore - blackPsqScore.pieceSquareScore;

    return score * (board.whiteToMove() ? 1 : -1);
}

MaterialEval Evaluator::EvaluateMaterial(Board& board, const bool forWhite, const float endGameRatio){
    const auto& pieces = forWhite ? whitePieces : blackPieces;
    MaterialEval eval{0,0};

    for (auto const& piece: pieces) {
        auto placements = board.getBoardByPiece(piece);
        const auto count = std::popcount(placements);
        eval.materialScore += count * pieceValues[piece.type()];
        while (placements) { const auto thisSquare = static_cast<Square>(std::countr_zero(placements));
            const auto startScore = getScoreOnSquare(piece.type(),thisSquare,forWhite,true);
            const auto endScore = getScoreOnSquare(piece.type(),thisSquare,forWhite,false);

            eval.pieceSquareScore += startScore * (1.0 - endGameRatio) + endScore * endGameRatio;

            placements &= placements -1;
        }
    }

    return eval;
}

// clang-format off
int Evaluator::getScoreOnSquare(const PieceType type, const Square onSquare, const bool isWhite, const bool endGame){
    const auto index = isWhite ? onSquare ^ 56 : onSquare;

    if (endGame) {
        switch (type) {
        case Pawn       : return  pawnTables[index].end;
        case Rook       : return  rookTables[index].end;
        case Knight     : return  knightTables[index].end;
        case Queen      : return  queenTables[index].end;
        case King       : return  kingTables[index].end;
        case Bishop     : return  bishopTables[index].end;
        default         : return  0;
        }
    }
    switch (type) {
        case Pawn       : return  pawnTables[index].start;
        case Rook       : return  rookTables[index].start;
        case Knight     : return  knightTables[index].start;
        case Queen      : return  queenTables[index].start;
        case King       : return  kingTables[index].start;
        case Bishop     : return  bishopTables[index].start;
        default         : return  0;
    }

    return 0;
}
// clang-format on