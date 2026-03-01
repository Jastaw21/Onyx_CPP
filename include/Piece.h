#ifndef ONYXCPP_PIECE_H
#define ONYXCPP_PIECE_H

#include <array>
#include <cstdint>

enum PieceType : uint8_t {
    Pawn=0, Knight=1, Bishop=2, Rook=3, Queen=4, King=5, NoType=6
};

enum Colour : uint8_t {
    NoColour, White, Black
};



struct Piece {
private:

    uint8_t value;

public:

    Piece() : value(0){}
    Piece(const PieceType type, const Colour colour) : value(type | colour << 4){}
    int index() const{return static_cast<int>(type()) + (isWhite() ? 0 : 6);}

    PieceType type() const{ return static_cast<PieceType>(value & 0x0F); }
    Colour colour() const{ return static_cast<Colour>(value >> 4); }

    bool exists() const{ return value != 0; }
    bool isWhite() const{ return value >> 4 == Colour::White;}

    uint8_t Value() const{ return value; }

    bool operator==(const Piece& other) const{ return other.value == this->value; }

};

// Pawn, Knight, Bishop, Rook, Queen, King
static inline  std::array<int,6> pieceValues = {100,300,320,500,900,0};


#endif //ONYXCPP_PIECE_H