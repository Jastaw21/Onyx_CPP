//
// Created by jacks on 08/02/2026.
//

#ifndef ONYXCPP_CLIBOT_H
#define ONYXCPP_CLIBOT_H
#include "SearchController.h"
#include "Searcher.h"
#include "UciParser.h"

template<typename... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

template<typename... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

class CliBot {
public:
    CliBot() : controller_(board_) {}

    // clang-format off
    void HandleCommand(const Command& cmd) {
        std::visit(Overloaded{
            [this](const UCICommand& c)        { onUCI(c); },
            [this](const GoCommand& c)         { onGo(c); },
            [this](const StopCommand& c)       { onStop(c); },
            [this](const QuitCommand& c)       { onQuit(c); },
            [this](const IsReadyCommand& c)    { onIsReady(c); },
            [this](const PositionCommand& c)   { onPosition(c); },
            [this](const NewGameCommand& c)    { onNewGame(c); },

        }, cmd);
    }
    // clang-format on

private:

    SearchController controller_;
    std::string ID = "OnyxCPP v0.0";
    Board board_;
    void onDepthComplete(SearchInfo& info);

    void onUCI(const UCICommand&);
    void onGo(const GoCommand&);
    void onStop(const StopCommand&);
    void onQuit(const QuitCommand&);
    void onIsReady(const IsReadyCommand&);
    void onPosition(const PositionCommand&);
    void onNewGame(const NewGameCommand&);

};


#endif //ONYXCPP_CLIBOT_H