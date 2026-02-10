//
// Created by jacks on 09/02/2026.
//

#ifndef ONYXCPP_SEARCHCONTROLLER_H
#define ONYXCPP_SEARCHCONTROLLER_H
#include <thread>

#include "Board.h"
#include "CancellationToken.h"
#include "Searcher.h"
#include "SearchThread.h"
#include "Timer.h"


class SearchController {
public:
    using InfoCallback = std::function<void(const SearchInfo&)>;
    SearchController(Board& board)
    : worker_(std::make_unique<SearchThread>(
          board,
          [this](const SearchInfo& info) {
              onDepthComplete(info);
          })) {}

    void start(SearchOptions& options) {
        timer_.start();
        worker_->Start(options);
    }

    void stop() {
        worker_->Stop();
    }

    SearchResults results() const {
        return worker_->getLastResults();
    }


private:
    CancellationToken token_;
    std::unique_ptr<SearchThread> worker_;
    InfoCallback cb;
    Timer timer_;


    void onDepthComplete(const SearchInfo& info){
        auto elapsed = timer_.elapsedMs();
        std::cout << "info depth " << info.depth << " multipv 1 " << "score cp " << info.bestScore << " nodes " << info.
                stats.nodes
                << " nps " << info.stats.nodes / elapsed * 1000.0 << " time " << elapsed
                << " beta cutoff " << info.stats.betaCutoffs << std::endl;
    }
};


#endif //ONYXCPP_SEARCHCONTROLLER_H