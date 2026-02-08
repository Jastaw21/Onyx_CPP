#include "include/Fen.h"
#include "include/PerftRunner.h"

int main(){

    auto board = Board(FenHelpers::KiwiPeteFen);
    auto e5g6 = moveFromNotation("e5g6");
    board.makeMove(e5g6);
    auto b6a4 = moveFromNotation("b6a4");
    board.makeMove(b6a4);

    auto g6h8 = moveFromNotation("g6h8");
    board.makeMove(g6h8);
    PerftRunner::PerftDivide(board,1    );
}