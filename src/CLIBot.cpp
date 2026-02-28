//
// Created by jacks on 08/02/2026.
//

#include "CLIBot.h"

#include <iostream>

#include "PerftRunner.h"

void CliBot::onDepthComplete(SearchInfo& info){

}

void CliBot::onUCI(const UCICommand&){
    std::cout << "id name " << ID << std::endl;
    std::cout << "id author JackWeddell" << std::endl;
    std::cout << "uciok" << std::endl;
}

void CliBot::onGo(const GoCommand& command){
    if (command.isPerft)
        PerftRunner::PerftDivide(board_,command.depth);
    else {
        SearchOptions opts;
        opts.depthLimit = command.depth;
        opts.tc = command.timeControl;
        controller_.start(opts);
    }
}
void CliBot::onStop(const StopCommand&){
    controller_.stop();
}
void CliBot::onQuit(const QuitCommand&){
    controller_.stop();
    std::exit(0);
}
void CliBot::onIsReady(const IsReadyCommand&){
    std::cout << "readyok" << std::endl;
}
void CliBot::onPosition(const PositionCommand& command){
    controller_.stop();
    board_.loadFen(command.fen);

    for (auto& move : command.moves) {
        Move copyMove = moveFromNotation(move);
        board_.addMoveFlags(copyMove);
        board_.makeMove(copyMove);
    }
}
void CliBot::onNewGame(const NewGameCommand&){}
void CliBot::onPrintDebug(PrintDebugCommand printDebugCommand){
    std::cout <<std::to_string( board_.getHash()) << " " << board_.getFen() << std::endl;
}