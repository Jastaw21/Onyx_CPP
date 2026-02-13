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
#include "TranspositionTable.h"


class SearchController {
public:

    using InfoCallback = std::function<void(const SearchInfo&)>;

    explicit SearchController(Board& board);
    void start(SearchOptions& options);
    void stop(){
        monitoring_ = false;

        worker_->Stop();

        if (timerThread_.joinable())
            timerThread_.join();
    }

    TranspositionTable& transpositionTable(){ return transpositionTable_; }
    SearchResults results() const{ return worker_->getLastResults(); }
    int getAge() const{ return age; }

private:

    CancellationToken token_;
    std::unique_ptr<SearchThread> worker_;
    InfoCallback cb;
    Timer timer_;
    Board& board_;
    TranspositionTable transpositionTable_;
    int age =0;

    std::thread timerThread_;
    std::atomic<bool> monitoring_{false};

    void monitorTime(){
        using namespace std::chrono_literals;

        while (monitoring_) {
            if (timer_.shouldStop()) {
                worker_->Stop();
                monitoring_ = false;
                return;
            }

            std::this_thread::sleep_for(2ms);
        }
    }


    void onDepthComplete(const SearchInfo& info);
    uint64_t getTimePerMove(SearchOptions& options);
};


#endif //ONYXCPP_SEARCHCONTROLLER_H