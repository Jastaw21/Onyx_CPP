//
// Created by jacks on 07/02/2026.
//

#ifndef ONYXCPP_PERFTRUNNER_H
#define ONYXCPP_PERFTRUNNER_H
#include <cstdint>

#include "Board.h"

class PerftRunner {
public:
    static uint64_t RunPerft(Board& board, int depth);
    static void PerftDivide(Board& board, int depth);

private:
    static uint64_t CountNodes(Board& board, int depth);
};


#endif //ONYXCPP_PERFTRUNNER_H