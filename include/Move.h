//
// Created by jacks on 06/02/2026.
//

#ifndef ONYXCPP_MOVE_H
#define ONYXCPP_MOVE_H
#include <cstdint>

#include "types.h"
#include "Piece.h"
#include "utils.h"

// clang-format off
enum MoveFlags : uint8_t {
    Quiet           = 0,        // 0000 0000
    Capture         = 1 << 0,   // 0000 0001
    EnPassant       = 1 << 1,   // 0000 0010
    Castling        = 1 << 2,   // 0000 0100
    PromotionQueen  = 1 << 3,   // 0000 1000
    PromotionRook   = 1 << 4,   // 0001 0000
    PromotionKnight = 1 << 5,   // 0010 0000
    PromotionBishop = 1 << 6,   // 0100 0000
};
// clang-format on


struct Move {
private:

    uint32_t data_;

public:

    Move() : data_(0){} // default constructor
    Move(const Square from, const Square to, const uint8_t flags) : data_(from | to << 6 | flags << 12){}


    bool isNullMove() const{ return data_ == 0; }
    uint8_t flags() const{ return data_ >> 12; }
    void addFlag(uint8_t flag) {auto shiftedFlag = flag << 12; data_ |= shiftedFlag;}
    Square from() const{ return data_ & 0x3f; }
    Square to() const{ return data_ >> 6 & 0x3f; }
    bool isPromotion() const{ return flags() & (PromotionQueen | PromotionBishop | PromotionKnight | PromotionRook); }

    PieceType promotedPiece() const{
        auto flag = flags();
        if (flag & PromotionQueen)
            return Queen;
        else if (flag & PromotionBishop)
            return Bishop;
        else if (flag & PromotionKnight)
            return Knight;
        else if (flag & PromotionRook)
            return Rook;
        return NoType;
    }

    bool operator==(const Move& other) const{return other.data_ == this->data_; }
};


#endif //ONYXCPP_MOVE_H