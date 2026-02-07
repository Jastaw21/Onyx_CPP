#include "../include/Board.h"

#include <numeric>

#include "../include/Fen.h"
#include "../include/Move.h"
#include "../include/utils.h"

void Board::setOff(const Piece piece, const Square square){
    // this will currently overwrite the piece that moved there
    board_[square] = Piece();
    boards_[piece.index()] &= ~(1ULL << square);
}

Board::Board(){
    board_ = std::array<Piece, 64>{Piece()};
    boards_ = std::array<Bitboard, 12>{0ULL};
}

Board::Board(const Fen& fen){
    board_ = std::array<Piece, 64>{Piece()};
    boards_ = std::array<Bitboard, 12>{0ULL};
    FromFen(fen);
}

void Board::updateCastlingRights(const Piece pieceMoved, const RankAndFile moveFrom){
    // moving the king loses that side's castling rights
    if (pieceMoved.type() == King) {
        if (pieceMoved.colour() == White)
            castlingRights_ &= ~(FenHelpers::WhiteKingside | FenHelpers::WhiteQueenside);
        else { castlingRights_ &= ~(FenHelpers::BlackKingside | FenHelpers::BlackQueenside); }
    }
    // moving a rook also loses castling rights
    if (pieceMoved.type() == Rook) {
        bool isKingSideLoss = moveFrom.file == 7;
        bool isQueenSideLoss = moveFrom.file == 0;
        if (pieceMoved.colour() == White) {
            if (isKingSideLoss)
                castlingRights_ &= ~FenHelpers::WhiteKingside;
            if (isQueenSideLoss)
                castlingRights_ &= ~FenHelpers::WhiteQueenside;
        } else {
            if (isKingSideLoss)
                castlingRights_ &= ~FenHelpers::BlackKingside;
            if (isQueenSideLoss)
                castlingRights_ &= ~FenHelpers::BlackQueenside;
        }
    }
}

void Board::makeMove(const Move& move_){
    Square moveFromSquare = move_.from();
    const auto pieceMoved = board_[moveFromSquare];

    // detect the capture - but consider ep
    Piece pieceCaptured;
    const bool isWhite = pieceMoved.colour() == White;
    const uint8_t flags = move_.flags();
    Square moveToSquare = move_.to();
    if (flags & EnPassant) {
        const Colour capturedColour = isWhite ? Black : White;
        pieceCaptured = Piece(Pawn, capturedColour);
    } else { pieceCaptured = board_[moveToSquare]; }

    const RankAndFile moveFrom = squareToRankAndFile(moveFromSquare);
    const RankAndFile moveTo = squareToRankAndFile(moveToSquare);

    pushHistory(pieceCaptured);

    // if the ep square is set, any move loses it
    const auto lastEPSquare = enPassantSquare_;
    if (enPassantSquare_ != -1)
        enPassantSquare_ = -1;

    updateCastlingRights(pieceMoved, moveFrom);

    // double push - creating an exposed en passant target square
    if (pieceMoved.type() == Pawn && std::abs(moveFromSquare - moveToSquare) == 16) {
        const auto targetEPRank = pieceMoved.isWhite() ? 2 : 5;

        enPassantSquare_ = rankAndFileToSquare(targetEPRank, moveFrom.file);
    }

    // remove the captured piece (doesn't do anything for ep)
    if (pieceCaptured.exists() && !(flags & EnPassant))
        setOff(pieceCaptured, moveToSquare);

    // if is ep we have to force it to work right
    if (flags & EnPassant) {
        const Square capturedOn = lastEPSquare + (pieceMoved.isWhite() ? -8 : 8);
        setOff(pieceCaptured, capturedOn);
    }

    // castling, the king sorts itself out, but need to do the rook manually
    if (flags & Castling) {
        const auto rookTargetFile = moveTo.file == 6 ? 5 : 3; // always moves one inside
        const auto rookOriginFile = moveTo.file == 6 ? 7 : 0;
        const Piece movedRook = isWhite ? Piece(Rook, White) : Piece(Rook, Black);
        setOff(movedRook, rankAndFileToSquare(moveFrom.rank, rookOriginFile));
        setOn(movedRook, rankAndFileToSquare(moveFrom.rank, rookTargetFile));
    }

    if (move_.isPromotion()) {
        setOff(pieceMoved, moveFromSquare);
        const auto promotedTo = Piece(move_.promotedPiece(), isWhite ? White : Black);
        setOn(promotedTo, moveToSquare);
    } else {
        // move the moving piece
        movePiece(pieceMoved, moveFromSquare, moveToSquare);
    }

    // flip turns
    whiteToMove_ = !whiteToMove_;

    // non-reversible moves reset the half move counter
    if (pieceCaptured.exists() || pieceMoved.type() == Pawn)
        halfMoves_ = 0;
    else
        halfMoves_++;

    // if black last moved (now white), we get another full move
    if (whiteToMove_)
        fullMoves_++;
}

void Board::unmakeMove(const Move& move){
    // restore history
    const auto lastState = history_.top();
    history_.pop();
    zobrist_ = lastState.hash;
    castlingRights_ = lastState.castlingRights;
    enPassantSquare_ = lastState.enPassantSquare;
    halfMoves_ = lastState.halfMoves;
    fullMoves_ = lastState.fullMoves;

    Square to = move.to();
    Square from = move.from();
    uint8_t flags = move.flags();

    // send the piece back
    const auto movedPiece = board_[to];

    if (move.isPromotion()) {
        const auto promotedPiece = Piece(move.promotedPiece(), movedPiece.isWhite() ? White : Black);
        const auto pawnToRestore = Piece(Piece(Pawn, movedPiece.isWhite() ? White : Black));
        setOff(promotedPiece, to);
        setOn(pawnToRestore, from);
    } else { movePiece(movedPiece, to, from); }

    // reset the captured piece (if a normal capture)
    if (lastState.capturedPiece.exists() && !(flags & EnPassant))
        setOn(lastState.capturedPiece, to);

    if (flags & EnPassant) {
        Square capturedOn = enPassantSquare_ + (movedPiece.isWhite() ? -8 : 8);
        setOn(lastState.capturedPiece, capturedOn);
    }

    if (flags & Castling) {
        auto moveTo = squareToRankAndFile(to);
        auto moveFrom = squareToRankAndFile(from);
        auto rookTargetFile = moveTo.file == 6 ? 5 : 3; // always moves one inside
        auto rookOriginFile = moveTo.file == 6 ? 7 : 0;
        Piece movedRook = movedPiece.isWhite() ? Piece(Rook, White) : Piece(Rook, Black);
        setOn(movedRook, rankAndFileToSquare(moveFrom.rank, rookOriginFile));
        setOff(movedRook, rankAndFileToSquare(moveFrom.rank, rookTargetFile));
    }

    // flip turns
    whiteToMove_ = !whiteToMove_;
}

Bitboard Board::getOccupancy(){ return std::accumulate(boards_.begin(), boards_.end(), 0ULL, std::bit_or<>()); }
void Board::loadFen(const Fen& fen){ FromFen(fen); }

Fen Board::getFen() const{
    Fen builtFen;

    // do the position fen
    for (int rank = 7; rank >= 0; rank--) {
        int numberOfEmpties = 0;
        for (int file = 0; file <= 7; file++) {
            const Square square = rankAndFileToSquare(rank, file);
            auto piece = board_[square];

            if (!piece.exists()) {
                numberOfEmpties++;
                if (file == 7) { builtFen += std::to_string(numberOfEmpties); }
                continue;
            }

            if (numberOfEmpties > 0) {
                builtFen += std::to_string(numberOfEmpties);
                numberOfEmpties = 0;
            }

            auto charOfThisPiece = pieceTypeToChar(piece.type());
            if (piece.isWhite())
                charOfThisPiece = std::toupper(charOfThisPiece);
            builtFen += charOfThisPiece;
        }
        if (rank != 0)
            builtFen += '/';
    }

    builtFen += " ";

    if (whiteToMove_)
        builtFen += "w ";
    else
        builtFen += "b ";

    std::string castlingString;

    if (castlingRights_ == 0)
        builtFen += "-";
    else {
        if (castlingRights_ & FenHelpers::WhiteKingside)
            builtFen += "K";
        if (castlingRights_ & FenHelpers::WhiteQueenside)
            builtFen += "Q";
        if (castlingRights_ & FenHelpers::BlackKingside)
            builtFen += "k";
        if (castlingRights_ & FenHelpers::BlackQueenside)
            builtFen += "q";
    }
    builtFen += " ";

    if (enPassantSquare_ == -1)
        builtFen += "-";
    else { builtFen += squareToNotation(enPassantSquare_); }

    builtFen += " ";
    builtFen += std::to_string(halfMoves_);
    builtFen += " ";
    builtFen += std::to_string(fullMoves_);

    return builtFen;
}

void Board::setOn(const Piece piece, const Square square){
    board_[square] = piece;
    boards_[piece.index()] |= 1ULL << square;
}

void Board::movePiece(const Piece piece, const Square from, const Square to){
    setOff(piece, from);
    setOn(piece, to);
}

void Board::FromFen(const Fen& fen){
    // reset the boards
    board_.fill(Piece());
    boards_.fill(0ULL);

    int rank = 7;
    int file = 0;
    size_t i = 0;

    const auto parsedFen = FenHelpers::ParseFen(fen);

    // handle the position
    while (i < parsedFen.positionFen.size() && parsedFen.positionFen[i] != ' ') {
        const char c = parsedFen.positionFen[i];

        // go to end of line, do a "carriage return"
        if (c == '/') {
            rank--;
            file = 0;
        }
        // got a skip to the next piece
        else if (isdigit(c)) { file += c - '0'; } else {
            const bool isWhite = isupper(c);
            const PieceType type = charToPieceType(c);
            const auto thisPiece = Piece(type, isWhite ? White : Black);
            const Square thisSquare = rankAndFileToSquare(rank, file);
            setOn(thisPiece, thisSquare);

            file++;
        }
        i++;
    }

    enPassantSquare_ = parsedFen.enPassantSquare;
    castlingRights_ = parsedFen.castlingRights;
    halfMoves_ = parsedFen.halfMoves;
    fullMoves_ = parsedFen.fullMoves;
}

void Board::pushHistory(const Piece capturedPiece){
    const BoardState currentState = {
                .hash = zobrist_, .castlingRights = castlingRights_, .enPassantSquare = enPassantSquare_,
                .halfMoves = halfMoves_, .fullMoves = fullMoves_, .capturedPiece = capturedPiece
            };
    history_.push(currentState);
}