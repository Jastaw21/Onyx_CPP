#include <iostream>

#include "CLIBot.h"
#include "include/Fen.h"
#include "include/PerftRunner.h"

int main(){
    CliBot engine;
    UCIParser parser;

    std::string line;

    // UCI engines read commands line-by-line from stdin
    while (std::getline(std::cin, line)) {
        if (line.empty())
            continue;

        auto cmd = parser.parse(line);
        if (cmd.has_value()) {
            engine.HandleCommand(cmd.value());

            // Optional: break on quit
            if (std::holds_alternative<QuitCommand>(cmd.value()))
                break;
        }
    }

    return 0;
}