//
// Created by jacks on 08/02/2026.
//

#ifndef ONYXCPP_UCIPARSER_H
#define ONYXCPP_UCIPARSER_H


#include <optional>
#include <string>
#include <variant>

#include <vector>

#include "TimeControl.h"
#include "UciTokeniser.h"


struct UCICommand {};

struct StopCommand {};

struct QuitCommand {};

struct IsReadyCommand {};

struct GoCommand {
    int depth = 128;
    TimeControl timeControl{};
    bool isPerft= false;
};

struct PositionCommand {
    bool isStartPos = true;
    std::string fen;
    std::vector<std::string> moves;
};

struct NewGameCommand {};

using Command = std::variant<
    UCICommand,
    GoCommand,
    StopCommand,
    QuitCommand,
    IsReadyCommand,
    PositionCommand,
    NewGameCommand
>;

class UCIParser {
public:

    std::optional<Command> parse(const std::string& parseTarget);

private:

    int currentToken = 0;
    std::vector<Token> tokens;

    Token peek();
    Token consume();

    std::optional<Command> parsePosition();
    std::optional<Command> parseGo();
    std::optional<Command> parseBestMove();

};



#endif //ONYXCPP_UCIPARSER_H