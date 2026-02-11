//
// Created by jacks on 09/02/2026.
//

#include "../include/SearchController.h"

void SearchController::onDepthComplete(const SearchInfo& info){
    const auto elapsed = std::max(1ull,timer_.elapsedMs());


    std::cout
            << "info depth " << info.depth << " multipv 1 " << "score cp " << info.bestScore << " nodes "
            << info.stats.nodes << " nps " << (int)(info.stats.nodes / elapsed * 1000.0) << " time " << elapsed
            << " pv " << info.pv << std::endl;
}

uint64_t SearchController::getTimePerMove(SearchOptions& options){
    bool hasTimeLimit = false;
    auto ourTime = board_.whiteToMove() ? options.tc.wtime : options.tc.btime;
    auto ourInc = board_.whiteToMove() ? options.tc.winc : options.tc.binc;

    auto plyDone = board_.fullMoves() * 2;

    int movesToGo = 0;
    if (plyDone < 20) movesToGo = 40;
    else if (plyDone <60) movesToGo = 60;
    else movesToGo = 20;

    if (options.tc.movestogo != 0) movesToGo = options.tc.movestogo;

    float baseTime = ourTime / movesToGo + ourInc * 0.5f;
    auto safeMax = ourTime * 0.2f;
    auto budget = std::min(safeMax,baseTime);
    return budget;
}