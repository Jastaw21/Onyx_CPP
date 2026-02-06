#include <iostream>
#include "include/Piece.h"


int main(){

    auto piece = Piece(PieceType::Pawn, Colour::White);
    std::cout << std::to_string(piece.Value());
}