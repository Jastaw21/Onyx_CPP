//
// Created by jacks on 08/02/2026.
//

#include "../include/UciTokeniser.h"

#include <algorithm>

Tokeniser::Tokeniser(const std::string& input){ tokenise(input); }


void Tokeniser::tokenise(const std::string& input){
    std::string token;

    for (const auto& c: input) {
        if (c == ' ') {
            // outsource handling
            handleToken(token);
        }

        // continue building the token
        else { token += c; }
    }

    // Handle the last token if input doesn't end with space
    if (!token.empty()) { handleToken(token); }

    tokens.push_back(Token{TokenType::EOF_TOKEN, ""}); // end with EOF
}

void Tokeniser::handleToken(std::string& builtToken){
    // search position
    TokenType type;
    if (builtToken == "position") { type = TokenType::POSITION; }
    // search move
    else if (builtToken == "moves") { type = TokenType::MOVES; }
    // search uci
    else if (builtToken == "uci") { type = TokenType::UCI; }
    // search go
    else if (builtToken == "go") { type = TokenType::GO; }
    // search quit
    else if (builtToken == "quit") { type = TokenType::QUIT; }
    // search isready
    else if (builtToken == "isready") { type = TokenType::ISREADY; }
    // search setoption
    else if (builtToken == "setoption") { type = TokenType::SETOPTION; }
    // search ucinewgame
    else if (builtToken == "ucinewgame") { type = TokenType::UCI_NEW_GAME; }
    // search stop
    else if (builtToken == "stop") { type = TokenType::STOP; }
    // search startpos
    else if (builtToken == "startpos") { type = TokenType::STARTPOS; }
    // search a '-' token
    else if (builtToken == "-") { type = TokenType::DASH; }
    // search depth
    else if (builtToken == "depth") { type = TokenType::DEPTH; }
    // w time
    else if (builtToken == "wtime") { type = TokenType::WTIME; }
    // b time
    else if (builtToken == "btime") { type = TokenType::BTIME; }
    // w inc
    else if (builtToken == "winc") { type = TokenType::WINC; }
    // b inc
    else if (builtToken == "binc") { type = TokenType::BINC; }
    else if (builtToken == "perft"){type = TokenType::PERFT;}
    else if (builtToken == "fen") {type = TokenType::FEN_TOKEN;}
    else if (builtToken == "d") {type = TokenType::DEBUG;}

    //unknown token
    else { type = getUnknownTokenType(builtToken); }

    tokens.emplace_back(type, builtToken);

    builtToken = ""; // empty the token
}

TokenType Tokeniser::getUnknownTokenType(const std::string& token){
    if (isMove(token))
        return TokenType::MOVE_VALUE;
    if (isPosition(token))
        return TokenType::FEN_BODY;
    if (isIntLiteral(token))
        return TokenType::INT_LITERAL;
    if (isStringLiteral(token))
        return TokenType::STRING_LITERAL;
    if (token == "\n" || token == "\r")
        return TokenType::EOF_TOKEN;

    return TokenType::UNKNOWN;
}

bool Tokeniser::isMove(const std::string& token){
    if (token.size() < 4)
        return false;
    bool isMove = false;

    // bestmove is invalid, i.e the engine can't find a move.
    if (token == "0000"){
        return true;
    }

    if (token.size() == 4 || token.size() == 5) { isMove = true; }

    // the fifth token must be a promotion move, check it's valid
    if (token.size() == 5) { const char fifth = token[4];
        if (fifth != 'Q' && fifth != 'q' && fifth != 'N' && fifth != 'n' && fifth != 'B' && fifth != 'b'
            && fifth != 'R' && fifth != 'r'
        ) { return false; }
    }
    // should be alpha, digit, alpha, digit
    if (!std::isalpha(token[0]) || !std::isalpha(token[2]) || !std::isdigit(token[1]) || !std::isdigit(token[3]))
        isMove = false;

    return isMove;
}


bool Tokeniser::isPosition(const std::string& token){
    // really weak check if it's a position
    int slashCount = 0;
    for (const auto& c: token) { if (c == '/') { slashCount++; } }
    return slashCount == 7;
}


bool Tokeniser::isIntLiteral(const std::string& token){
    return std::ranges::all_of(token, [](const char c) { return std::isdigit(static_cast<unsigned char>(c)); });
}

bool Tokeniser::isStringLiteral(const std::string& token){
    return std::ranges::all_of(token, [](const char c) { return std::isalpha(static_cast<unsigned char>(c)); });
}