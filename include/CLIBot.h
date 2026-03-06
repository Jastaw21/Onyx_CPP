//
// Created by jacks on 08/02/2026.
//

#ifndef ONYXCPP_CLIBOT_H
#define ONYXCPP_CLIBOT_H
#include "Options.h"
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

    CliBot() : controller_(board_){
        Option lmrThreshold = {
                    .name = "lmrThreshold",
                    .value = 5,
                    .min = 1,
                    .max = 7,
                    .defaultValue = 5,
                    .isValid = true
                };
        options_.addOption(lmrThreshold);
    }

    Board& GetBoard(){ return board_; }


    void onSetOption(const SetOptionCommand& setOptionCommand);

    // clang-format off
    void HandleCommand(const Command& cmd){

        std::visit(Overloaded{
               [this](const UCICommand& c)          { onUCI(c); },
               [this](const GoCommand& c)           { onGo(c); },
               [this](const StopCommand& c)         { onStop(c); },
               [this](const QuitCommand& c)         { onQuit(c); },
               [this](const IsReadyCommand& c)      { onIsReady(c); },
               [this](const PositionCommand& c)     { onPosition(c); },
               [this](const NewGameCommand& c)      { onNewGame(c); },
               [this](const PrintDebugCommand& c)   { onPrintDebug(c); },
               [this](const SetOptionCommand& c)    { onSetOption(c); }
        }, cmd);
    }

    // clang-format on

private:

    Board board_;
    SearchController controller_;
    std::string ID = "OnyxCPP v0.0";
    static void onDepthComplete(SearchInfo& info);

    void onUCI(const UCICommand&) const;
    void onGo(const GoCommand&);
    void onStop(const StopCommand&);
    void onQuit(const QuitCommand&);
    static void onIsReady(const IsReadyCommand&);
    void onPosition(const PositionCommand&);
    static void onNewGame(const NewGameCommand&);
    void onPrintDebug(PrintDebugCommand printDebugCommand) const;

    Options options_{};
    void PushOptions(){ controller_.PushOptions(options_); }
};


#endif //ONYXCPP_CLIBOT_H