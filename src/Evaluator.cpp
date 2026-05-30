//
// Created by jacks on 08/02/2026.
//

#include "Evaluator.h"

#include "MagicBitboards.h"

std::array<Piece, 6> Evaluator::whitePieces = {
            Piece(Pawn, White),
            Piece(Knight, White),
            Piece(Bishop, White),
            Piece(Rook, White),
            Piece(Queen, White),
            Piece(King, White)
        };
std::array<Piece, 6> Evaluator::blackPieces = {
            Piece(Pawn, Black),
            Piece(Knight, Black),
            Piece(Bishop, Black),
            Piece(Rook, Black),
            Piece(Queen, Black),
            Piece(King, Black)
        };
int Evaluator::kingShieldPenalty = 10;
int Evaluator::openfilePenalty = 10;

constexpr int queenEndgameWeight = 45;
constexpr int rookEndgameWeight = 20;
constexpr int bishopEndgameWeight = 10;
constexpr int knightEndgameWeight = 10;

constexpr int startPieceValue =
        2 * rookEndgameWeight +
        2 * knightEndgameWeight +
        2 * bishopEndgameWeight +
        queenEndgameWeight;

float actualPieceValue(const MaterialRecord& material){
    const auto actVal =
            material.rookCount * rookEndgameWeight +
            material.queenCount * queenEndgameWeight +
            material.bishopCount * bishopEndgameWeight +
            material.knightCount * knightEndgameWeight;

    return  std::min(1.0f, actVal / static_cast<float>(startPieceValue));
}

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

    MaterialRecord whiteMaterial;
    PopulateMaterialRecord(board,true,whiteMaterial);

    MaterialRecord blackMaterial;
    PopulateMaterialRecord(board,false,blackMaterial);

    const auto whiteMaterialValue = MaterialScore(whiteMaterial);
    const auto blackMaterialValue = MaterialScore(blackMaterial);
    score += whiteMaterialValue - blackMaterialValue;

    const auto whiteEndGameRatio = actualPieceValue(whiteMaterial);
    const auto blackEndGameRatio = actualPieceValue(blackMaterial);

    const auto whitePsqScore = PieceSquareScore(true, whiteMaterial, whiteEndGameRatio);
    const auto blackPsqScore = PieceSquareScore(false, blackMaterial, blackEndGameRatio);
    score += whitePsqScore - blackPsqScore;

    const auto w_kssScore = KingOpenFileScore(true, board, whiteMaterial);
    const auto b_kssScore = KingOpenFileScore(false, board, blackMaterial);
    score += w_kssScore - b_kssScore;

    const auto wPP = PassedPawnScore(true, whiteMaterial, blackMaterial);
    const auto bPP = PassedPawnScore(false,whiteMaterial,blackMaterial);
    score += wPP - bPP;

    // normalise to side to move
    return score * (board.whiteToMove() ? 1 : -1);
}

int Evaluator::MaterialScore(const MaterialRecord& relevantMaterial){

    int pieceIdx= 0;
    int materialScore = 0;

    for (const auto value : pieceValues) {
        switch (pieceIdx) {
            case 0:
                materialScore+= relevantMaterial.pawnCount * value;
                break;
            case 1:
                materialScore+= relevantMaterial.knightCount * value;
                break;
            case 2:
                materialScore+= relevantMaterial.bishopCount * value;
                break;
            case 3:
                materialScore+= relevantMaterial.rookCount * value;
                break;
            case 4:
                materialScore+= relevantMaterial.queenCount * value;
                break;
            case 5:
                materialScore+= relevantMaterial.kingCount * value;
                break;
            default: break;
        }
        pieceIdx++;
    }
    return materialScore;
}

int Evaluator::PieceSquareScore(const bool forWhite, const MaterialRecord& relevantMaterial,
                                        const float endGameScore){
    int pieceIdx = 0;
    int pieceSquareScore = 0;
    auto pieces = forWhite ? whitePieces : blackPieces;

    for (const auto piece: pieces) {
        Bitboard placements = 0ULL;
        switch (pieceIdx) {
            case 0:
                placements = relevantMaterial.Pawns;
                break;
            case 1:
                placements = relevantMaterial.Knight;
                break;
            case 2:
                placements = relevantMaterial.Bishop;
                break;
            case 3:
                placements = relevantMaterial.Rook;
                break;
            case 4:
                placements = relevantMaterial.Queen;
                break;
            case 5:
                placements = relevantMaterial.King;
                break;
            default: break;
        }

        const auto pieceType = piece.type();
        const auto& squareScores = getTableByPieceType(pieceType); // get the table once for each piece
        while (placements) {
            const auto thisSquare = static_cast<Square>(std::countr_zero(placements));
            const auto index = forWhite ? thisSquare ^ 56 : thisSquare;
            const auto startScore = squareScores[index].start;

            const auto endScore = squareScores[index].end;

            pieceSquareScore += startScore * endGameScore + endScore * (1.0f - endGameScore);

            placements &= placements - 1;
        }

        pieceIdx++;
    }

    return pieceSquareScore;
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

int Evaluator::KingOpenFileScore(const bool forWhite, const Board& board, const MaterialRecord& material){

    if (material.King == 0ULL) return 0; // No king
    const auto asSquare = static_cast<Square>(std::countr_zero(material.King));

    const auto piecesAheadOnFile = countOccupantsForward(forWhite, asSquare, board.getOccupancy(Piece(Pawn, White)) | board.getOccupancy(Piece(Pawn,Black)));
    if (piecesAheadOnFile == 0)
        return -openfilePenalty; // penalty for an open file
    return 0;
}

int Evaluator::PassedPawnScore(const bool forWhite, const MaterialRecord& whiteMaterial, const MaterialRecord& blackMaterial){

    Bitboard relevantPawns = forWhite ? whiteMaterial.Pawns : blackMaterial.Pawns;
    const Bitboard pawnMask = forWhite ? blackMaterial.Pawns : whiteMaterial.Pawns;
    int totalPassedPawns = 0;

    while (relevantPawns) {

        auto possFiles = 0;
        auto validFiles = 0;

        const auto passedPawnSquare = static_cast<Square>(std::countr_zero(relevantPawns));
        const auto raf = squareToRankAndFile(passedPawnSquare);
        // check file left
        if (raf.file > 0) {
            possFiles++;
            const auto leftSquare = passedPawnSquare - 1; // (left from white's perspective
            const int fl = countOccupantsForward(forWhite, leftSquare,pawnMask);
            if (fl == 0) validFiles++;

        }
        // check same file
        possFiles++;
        const int fc = countOccupantsForward(forWhite, passedPawnSquare,pawnMask);
        if (fc == 0) validFiles++;

        // check file right
        if (raf.file < 7) {
            possFiles++;
            const auto rightSquare = passedPawnSquare + 1; // (left from white's perspective
            const int fr = countOccupantsForward(forWhite, rightSquare,pawnMask);
            if (fr == 0) validFiles++;
        }


        if (possFiles == validFiles)
            totalPassedPawns += 1;

        relevantPawns &= relevantPawns -1;
    }

    return totalPassedPawns * 12; // a pawn if we have 8 passed pawns
}

void Evaluator::PopulateMaterialRecord(const Board& board, const bool forWhite, MaterialRecord& outMaterial){
    const auto& pieces = forWhite ? whitePieces : blackPieces;

    int pieceIdx = 0;

    // material score loop
    for (const Piece piece: pieces) {
        auto placements = board.getOccupancy(piece);
        const auto count = std::popcount(placements);

        // we'll use these placements later in the eval function, may as well cache them to avoid loads of calls to getOccupancy
        switch (pieceIdx) {
            case 0:
                outMaterial.Pawns = placements;
                outMaterial.pawnCount += count;
                break;
            case 1:
                outMaterial.Knight = placements;
                outMaterial.knightCount += count;
                break;
            case 2:
                outMaterial.Bishop = placements;
                outMaterial.bishopCount += count;
                break;
            case 3:
                outMaterial.Rook = placements;
                outMaterial.rookCount += count;
                break;
            case 4:
                outMaterial.Queen = placements;
                outMaterial.queenCount += count;
                break;
            case 5:
                outMaterial.King = placements;
                break;
            default: break;
        };

        pieceIdx++;
    }
}