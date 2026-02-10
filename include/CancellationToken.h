//
// Created by jacks on 10/02/2026.
//

#ifndef ONYXCPP_CANCELLATIONTOKEN_H
#define ONYXCPP_CANCELLATIONTOKEN_H
#include <atomic>

struct CancellationToken {

public:
    bool isStopped() const {return stopped_.load(std::memory_order_relaxed);}
    void stop() {stopped_.store(true,std::memory_order_relaxed);}
    void reset() {stopped_.store(false, std::memory_order_relaxed);}

private:
    std::atomic<bool> stopped_{false};


};


#endif //ONYXCPP_CANCELLATIONTOKEN_H