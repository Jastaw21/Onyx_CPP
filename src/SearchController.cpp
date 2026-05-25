//
// Created by jacks on 09/02/2026.
//

#include "../include/SearchController.h"

#include "Evaluator.h"
#include  "Options.h"

SearchController::SearchController(Board& board) : board_(board), transpositionTable_(512), worker_(
                                                       std::make_unique<SearchThread>(
                                                           board, this, [this](const SearchInfo& info) {
                                                               onDepthComplete(info);
                                                           })){}


void SearchController::start(const SearchOptions& options){
    if (timerThread_.joinable())
        timerThread_.join();

    const auto timePerMove = getTimePerMove(options);

    timer_.start(timePerMove);
    worker_->Start(options);

    if (options.tc.isTimed()) {
        monitoring_ = true;
        timerThread_ = std::thread(&SearchController::monitorTime, this);
    }

    age++;
}

void SearchController::PushOptions(Options& options) const{
    // applies the set options

    const auto& lmrOption = options["lmrThreshold"];
    worker_->GetSearcher().LMRThreshold = std::get<int>(lmrOption.value);

    const auto& contemptOption = options["contempt"];
    worker_->GetSearcher().contempt = std::get<int>(contemptOption.value);

    const auto& deepLMR = options["deepLMR"];
    worker_->GetSearcher().DeepLMR = std::get<int>(deepLMR.value);

    const auto& kingShieldSet = options["kspen"];
    Evaluator::kingShieldPenalty = std::get<int>(kingShieldSet.value);

    const auto& ofPen = options["ofpen"];
    Evaluator::openfilePenalty = std::get<int>(ofPen.value);
}


void SearchController::onNewGame(){ transpositionTable_.Reset(); }


void SearchController::onDepthComplete(const SearchInfo& info) const{
    const auto te = timer_.elapsedMs();
    const auto elapsed = te < 1 ? 1 : te; // clamp to 1 to avoid div by 0

    std::cout
            << "info depth " << info.depth << " multipv 1 " << "score cp " << info.bestScore << " nodes "
            << info.stats.nodes << " nps " << static_cast<int>(info.stats.nodes / elapsed * 1000.0) << " time " <<
            elapsed
            << " pv " << info.pv << std::endl;
}

uint64_t SearchController::getTimePerMove(const SearchOptions& options) const{
    bool hasTimeLimit = false;
    const auto ourTime = board_.whiteToMove() ? options.tc.wtime : options.tc.btime;
    const auto ourInc = board_.whiteToMove() ? options.tc.winc : options.tc.binc;

    const auto plyDone = board_.fullMoves() * 2;

    int movesToGo = 0;
    if (plyDone < 20) movesToGo = 40;
    else if (plyDone < 60) movesToGo = 60;
    else movesToGo = 20;

    if (options.tc.movestogo != 0) movesToGo = options.tc.movestogo;

    const float baseTime = ourTime / movesToGo + ourInc * 0.5f;
    const auto safeMax = ourTime * 0.2f;
    const auto budget = std::min(safeMax, baseTime);
    return budget;
}