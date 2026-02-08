//
// Created by jacks on 08/02/2026.
//

#include "CLIBot.h"

#include <iostream>

#include "PerftRunner.h"

void CliBot::onUCI(const UCICommand&){
    std::cout << "id name " << ID << std::endl;
    std::cout << "id author JackWeddell" << std::endl;
    std::cout << "uciok" << std::endl;
}

void CliBot::onGo(const GoCommand& command){
    if (command.isPerft)
        PerftRunner::PerftDivide(worker_.GetBoard(),command.depth);
    else {
        SearchOptions opts;
        opts.depthLimit = command.depth;
        opts.tc = command.timeControl;
        auto result = worker_.search(opts);
        std::cout << "bestmove " << moveToNotation(result.bestMove) << std::endl;
    }
}
void CliBot::onStop(const StopCommand&){}
void CliBot::onQuit(const QuitCommand&){}
void CliBot::onIsReady(const IsReadyCommand&){
    std::cout << "readyok" << std::endl;
}
void CliBot::onPosition(const PositionCommand& command){
    worker_.GetBoard().loadFen(command.fen);

    for (auto& move : command.moves) {
        Move copyMove = moveFromNotation(move);
        worker_.GetBoard().addMoveFlags(copyMove);
        worker_.GetBoard().makeMove(copyMove);
    }
}
void CliBot::onNewGame(const NewGameCommand&){}