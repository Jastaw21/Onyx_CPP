//
// Created by jacks on 08/02/2026.
//

#include "Evaluator.h"

#include "MagicBitboards.h"


std::array<Piece, 6> Evaluator::whitePieces = {
            Piece(Pawn, White), Piece(King, White), Piece(Knight, White), Piece(Queen, White), Piece(Rook, White),
            Piece(Bishop, White)
        };
std::array<Piece, 6> Evaluator::blackPieces = {
            Piece(Pawn, Black), Piece(King, Black), Piece(Knight, Black), Piece(Queen, Black), Piece(Rook, Black),
            Piece(Bishop, Black)
        };

int Evaluator::kingShieldPenalty = 10;
int Evaluator::openfilePenalty = 10;

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
int Evaluator::Evaluate(const Board& board){

    int score = 0;


    Material outMaterial;
    const auto whitePsqScore = EvaluateMaterial(board, true, 0, outMaterial);
    const auto blackPsqScore = EvaluateMaterial(board, false, 0, outMaterial);
    const auto w_kssScore = KingSafetyScore(true, board, outMaterial);
    const auto b_kssScore = KingSafetyScore(false, board, outMaterial);


    score += whitePsqScore.materialScore - blackPsqScore.materialScore;
    score += whitePsqScore.pieceSquareScore - blackPsqScore.pieceSquareScore;

    score += w_kssScore - b_kssScore;

    return score * (board.whiteToMove() ? 1 : -1);
}

MaterialEval Evaluator::EvaluateMaterial(const Board& board, const bool forWhite, const float endGameRatio, Material& outMaterial){
    const auto& pieces = forWhite ? whitePieces : blackPieces;
    MaterialEval eval{0, 0};

    // we'll use this to check the pawns and kings to populate the eval, as they're the first two in the lists
    int pieceIdx = 0;
    for (auto const& piece: pieces) {
        auto placements = board.getOccupancy(piece);

        // we'll use these placements later in the eval function, may as well cache them to avoid loads of calls to getOccupancy
        if (pieceIdx == 0) {
            if (forWhite) outMaterial.whitePawns = placements;
            else outMaterial.blackPawns = placements;
        }
        else if (pieceIdx == 1) {
            if (forWhite) outMaterial.whiteKing = placements;
            else outMaterial.blackKing = placements;
        }

        // get the material score
        const auto count = std::popcount(placements);
        const auto pieceType = piece.type();
        eval.materialScore += count * pieceValues[pieceType];

        // now the piece square score
        const auto& squareScores = getTableByPieceType(pieceType); // get the table once for each piece
        while (placements) {
            const auto thisSquare = static_cast<Square>(std::countr_zero(placements));
            const auto index = forWhite ? thisSquare ^ 56 : thisSquare;
            const auto startScore = squareScores[index].start;

            if (endGameRatio > 0.001f) {
                const auto endScore = squareScores[index].end;

                eval.pieceSquareScore += startScore * (1.0 - endGameRatio) + endScore * endGameRatio;
            } else { eval.pieceSquareScore += startScore; }

            placements &= placements - 1;
        }


        pieceIdx++;
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

// ReSharper disable once CppNotAllPathsReturnValue
Psq& Evaluator::getTableByPieceType(const PieceType type){
	switch (type) {
		case Pawn       : return  pawnTables;
		case Rook       : return  rookTables;
		case Knight     : return  knightTables;
		case Queen      : return  queenTables;
		case King       : return  kingTables;
		case Bishop     : return  bishopTables;
        default: ;
    }
}

// clang-format on

int Evaluator::KingSafetyScore(const bool forWhite, const Board& board, const Material& material){

    return KingOpenFileScore(forWhite, board, material);
}

int Evaluator::KingShieldScoreByColour(const bool forWhite, const Board& board){
    const auto piece = Piece(King, forWhite ? White : Black);
    const auto location = board.getOccupancy(piece);
    if (location == 0ULL) return 0; // should never have no king but be careful.
    const auto asSquare = static_cast<Square>(std::countr_zero(location));

    const auto shield = MagicBitboards::getKingShield(forWhite, asSquare);
    const auto numPossShields = std::popcount(shield);
    const auto relevantPawn = Piece(Pawn, forWhite ? White : Black);
    const auto actualPawnLocs = board.getOccupancy(relevantPawn);

    const auto numActShields = std::popcount(actualPawnLocs & shield);
    return (numPossShields - numActShields) * -kingShieldPenalty;
}

int Evaluator::KingOpenFileScore(const bool forWhite, const Board& board, const Material& material){

    const auto location =  forWhite ? material.whiteKing : material.blackKing;
    if (location == 0ULL) return 0; // No king
    const auto asSquare = static_cast<Square>(std::countr_zero(location));

    const auto piecesAheadOnFile = countOccupantsForward(forWhite, asSquare, material.blackPawns | material.whitePawns);
    if (piecesAheadOnFile == 0)
        return -openfilePenalty; // penalty for an open file
    return 0;
}