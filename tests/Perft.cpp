//
// Created by jacks on 08/02/2026.
//

#include <complex>
#include <gtest/gtest.h>

#include "Fen.h"
#include "PerftRunner.h"

constexpr int MAXDEPTH = 5;
TEST(Perft,Startpos){
    uint64_t expectedResults[5] = {20,400,8902,197281,4865609};
    auto board = Board();
    int depth = 1;
    while (depth <= MAXDEPTH) {
        auto result = PerftRunner::RunPerft(board,depth);
        auto resultIndex = depth -1;
        EXPECT_EQ(expectedResults[resultIndex], result);
        std::cout << "Depth: " << depth << " " << result << "\n";
        depth ++;
    }
}

TEST(Perft,KiwiPete){
    uint64_t expectedResults[5] = {48,2039,97862,4085603,193690690};
    auto board = Board(FenHelpers::KiwiPeteFen);
    int depth = 1;
    while (depth <= MAXDEPTH) {
        auto result = PerftRunner::RunPerft(board,depth);
        auto resultIndex = depth -1;
        EXPECT_EQ(expectedResults[resultIndex], result);
        std::cout << "Depth: " << depth << " " << result << "\n";
        depth ++;
    }
}