//
// Created by jacks on 08/02/2026.
//

#ifndef ONYXCPP_TIMECONTROL_H
#define ONYXCPP_TIMECONTROL_H

struct TimeControl {
    int wtime;
    int btime;
    int winc;
    int binc;
    int movestogo;
    bool isTimed(){
        return wtime >0 || btime >0;
    }
};

#endif //ONYXCPP_TIMECONTROL_H