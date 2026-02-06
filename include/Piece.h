#ifndef ONYXCPP_PIECE_H
#define ONYXCPP_PIECE_H

#include <cstdint>

enum PieceType : uint8_t {
    Pawn, Knight, Bishop, Rook, Queen, King, NoType
};

enum Colour : uint8_t {
    NoColour, White, Black
};

struct Piece {
private:

    uint8_t value;

public:

    Piece() : value(0){}
    Piece(const PieceType type, const Colour colour) : value(type | (colour << 4)){}
    int index() const{return static_cast<int>(type()) + (isWhite() ? 0 : 6);}

    PieceType type() const{ return static_cast<PieceType>(value & 0x0F); }
    Colour colour() const{ return static_cast<Colour>(value >> 4); }

    bool exists() const{ return value != 0; }
    bool isWhite() const{ return value >> 4 == Colour::White;}

    uint8_t Value() const{ return value; }

    bool operator==(const Piece& other) const{ return other.value == this->value; }
};

#endif //ONYXCPP_PIECE_H