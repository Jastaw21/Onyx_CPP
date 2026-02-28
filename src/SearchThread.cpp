//
// Created by jacks on 10/02/2026.
//

#include "../include/SearchThread.h"
#include  "SearchController.h"

void SearchThread::loop(){
    while (true) {
        SearchOptions options; {
            std::unique_lock lock(mutex);
            cv.wait(lock, [&] { return searching || exiting; });

            if (exiting)
                return;

            options = options_;
        }
        const auto res = searcher_.search(options);
        std::cout << "bestmove " << moveToNotation(res.bestMove) << std::endl;

        //controller_->transpositionTable().Log();
        searcher_.Log(); {
            std::lock_guard lock(mutex);
            lastResults = res;
            searching = false;
        }
    }
}