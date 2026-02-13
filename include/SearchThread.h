//
// Created by jacks on 10/02/2026.
//

#ifndef ONYXCPP_SEARCHTHREAD_H
#define ONYXCPP_SEARCHTHREAD_H
#include <condition_variable>
#include <iostream>
#include <thread>
#include <utility>

#include "Board.h"
#include "CancellationToken.h"
#include "Searcher.h"

class SearchController;

class SearchThread {
public:

    SearchThread(Board& board, SearchController* controller,
                 std::function<void(const SearchInfo&)> callback = nullptr) : board_(board),
                                                                              searcher_(board, token_, controller,std::move(callback)),
                                                                              thread(&SearchThread::loop, this), controller_(controller)
    {}

    void Start(SearchOptions& options){ {
            std::lock_guard lock(mutex);
            options_ = options;
            token_.reset(); // get a new token
            searching = true;
        }
        cv.notify_one();
    }

    void Stop(){ token_.stop(); }
    SearchResults getLastResults() const{ return lastResults; }

private:

    void loop();


    CancellationToken token_;
    Board& board_;
    Searcher searcher_;
    SearchOptions options_;
    SearchResults lastResults;


    std::thread thread;
    std::condition_variable cv;
    std::mutex mutex;

    bool searching = false;
    bool exiting = false;
    SearchController* controller_;
};


#endif //ONYXCPP_SEARCHTHREAD_H