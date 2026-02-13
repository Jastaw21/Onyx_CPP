//
// Created by jacks on 09/02/2026.
//

#include "../include/SearchController.h"

SearchController::SearchController(Board& board) : worker_(std::make_unique<SearchThread>(
                                                       board, this, [this](const SearchInfo& info) {
                                                           onDepthComplete(info);
                                                       })),
                                                   board_(board), transpositionTable_(512){}


void SearchController::start(SearchOptions& options){
    if (timerThread_.joinable())
        timerThread_.join();

    auto timePerMove = getTimePerMove(options);

    timer_.start(timePerMove);
    worker_->Start(options);

    if (options.tc.isTimed()) {
        monitoring_ = true;
        timerThread_ = std::thread(&SearchController::monitorTime, this);
    }

    age++;
}

void SearchController::onDepthComplete(const SearchInfo& info){
    const auto elapsed = std::max(1ull, timer_.elapsedMs());

    std::cout
            << "info depth " << info.depth << " multipv 1 " << "score cp " << info.bestScore << " nodes "
            << info.stats.nodes << " nps " << (int) (info.stats.nodes / elapsed * 1000.0) << " time " << elapsed
            << " pv " << info.pv << std::endl;
}

uint64_t SearchController::getTimePerMove(SearchOptions& options){
    bool hasTimeLimit = false;
    auto ourTime = board_.whiteToMove() ? options.tc.wtime : options.tc.btime;
    auto ourInc = board_.whiteToMove() ? options.tc.winc : options.tc.binc;

    auto plyDone = board_.fullMoves() * 2;

    int movesToGo = 0;
    if (plyDone < 20) movesToGo = 40;
    else if (plyDone < 60) movesToGo = 60;
    else movesToGo = 20;

    if (options.tc.movestogo != 0) movesToGo = options.tc.movestogo;

    float baseTime = ourTime / movesToGo + ourInc * 0.5f;
    auto safeMax = ourTime * 0.2f;
    auto budget = std::min(safeMax, baseTime);
    return budget;
}