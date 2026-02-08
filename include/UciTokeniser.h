//
// Created by jacks on 08/02/2026.
//

#ifndef ONYXCPP_UCITOKENISER_H
#define ONYXCPP_UCITOKENISER_H


#include <string>
#include <vector>

enum class TokenType {
    // simple interactions
    UCI, QUIT, ISREADY, UCI_NEW_GAME, STOP,
    // move setting
    POSITION, STARTPOS, FEN_BODY, MOVES, MOVE_VALUE,

    // go and it's options
    GO, DEPTH,

    // misc engine options

     SETOPTION, NAME, VALUE,

    // literals
    INT_LITERAL, STRING_LITERAL, DASH,

    // parser utilities
    EOF_TOKEN, UNKNOWN,

    // time control
    WTIME, BTIME, WINC, BINC,
    PERFT,
    FEN_TOKEN,
};

struct Token {
    TokenType type;
    std::string value;
};

class Tokeniser {
public:

    explicit Tokeniser(const std::string& input);
    std::vector<Token>& getTokens(){ return tokens; }

    static bool isMove(const std::string& token);
    static bool isPosition(const std::string& token);
    static bool isIntLiteral(const std::string& token);
    static bool isStringLiteral(const std::string& token);

private:

    void tokenise(const std::string& input);
    std::vector<Token> tokens;
    void handleToken(std::string& builtToken);
    static TokenType getUnknownTokenType(const std::string& token);
};

#endif //ONYXCPP_UCITOKENISER_H