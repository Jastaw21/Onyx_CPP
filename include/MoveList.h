//
// Created by jacks on 07/02/2026.
//

#ifndef ONYXCPP_MOVELIST_H
#define ONYXCPP_MOVELIST_H
#include <algorithm>
#include <array>

#include "Move.h"


class MoveList {
std::array<Move, 256> moves_;
    size_t count_ = 0;

public:
    void add(Move move) {moves_[count_] = move; count_++;}
    size_t size() const{return count_;}
    Move& operator[](size_t i) { return moves_[i]; }
    const Move& operator[](size_t i) const { return moves_[i]; }
    bool contains(Move& move) {return std::ranges::contains(moves_,move);}

    Move* begin() { return moves_.data(); }
    Move* end() { return moves_.data() + count_; }
    const Move* begin() const { return moves_.data(); }
    const Move* end() const { return moves_.data() + count_; }

    void clear() { count_ = 0; }

};


#endif //ONYXCPP_MOVELIST_H