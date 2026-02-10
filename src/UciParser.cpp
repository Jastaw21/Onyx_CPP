//
// Created by jacks on 08/02/2026.
//

#include "UciParser.h"

#include <iostream>
#include <optional>
#include <string>


std::optional<Command> UCIParser::parse(const std::string& inString){
    currentToken = 0;
    auto tokeniser = Tokeniser(inString);
    tokens = tokeniser.getTokens();

    while (currentToken < tokens.size()) {
        const Token liveToken = consume();

        if (liveToken.type == TokenType::UNKNOWN) { break; }

        if (liveToken.type == TokenType::UCI) { return UCICommand{}; }

        if (liveToken.type == TokenType::STOP) { return StopCommand{}; }

        if (liveToken.type == TokenType::QUIT) { return QuitCommand{}; }

        if (liveToken.type == TokenType::ISREADY) { return IsReadyCommand{}; }

        if (liveToken.type == TokenType::UCI_NEW_GAME) { return NewGameCommand{}; }

        if (liveToken.type == TokenType::GO) { return parseGo(); }

        if (liveToken.type == TokenType::POSITION) { return parsePosition(); }


    }

    return std::nullopt; // no valid command found
}


Token UCIParser::peek(){
    if (currentToken < tokens.size()) { return tokens.at(currentToken); }
    return Token{.type = TokenType::EOF_TOKEN, .value = ""};
}

Token UCIParser::consume(){
    Token returnToken = peek();
    currentToken++;
    return returnToken;
}

std::optional<Command> UCIParser::parsePosition(){
    // after getting the position command, the next token can be startpos or a fen
    PositionCommand result;
    if (peek().type == TokenType::STARTPOS) {
        result.isStartPos = true;
        result.fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        consume();
    }
    // if we don't get a startpos, means we need to try and parse the fen


    else if (peek().type == TokenType::FEN_TOKEN) {
        consume(); // clear the fen token

        const std::string fenBody = peek().value;
        consume();
        result.isStartPos = false;
        result.fen = fenBody;

        while (peek().type == TokenType::STRING_LITERAL
               || peek().type == TokenType::INT_LITERAL
               || peek().type == TokenType::DASH
        ) { result.fen += ' ' + consume().value; }
    }

    if (peek().type == TokenType::MOVES) {
        consume();
        while (peek().type == TokenType::MOVE_VALUE) { result.moves.emplace_back(consume().value); }
    }

    return result;
}

std::optional<Command> UCIParser::parseGo(){
    GoCommand result;
    result.depth = 128;

    TimeControl tc{0,0,0,0,0};


    // while our "anchor token is a time-related one"
    while (
        peek().type == TokenType::GO
        || peek().type == TokenType::DEPTH
        || peek().type == TokenType::WTIME
        || peek().type == TokenType::BTIME
        || peek().type == TokenType::WINC
        || peek().type == TokenType::BINC
        || peek().type == TokenType::PERFT
    ) { const auto anchorToken = consume();
        switch (anchorToken.type) {
            case TokenType::DEPTH: {
                if (peek().type == TokenType::INT_LITERAL) { result.depth = std::stoi(consume().value); }
                break; // break from cases
            }

            case TokenType::PERFT: {
                result.isPerft = true;
                if (peek().type == TokenType::INT_LITERAL) { result.depth = std::stoi(consume().value); }
                break;
            }

            case TokenType::WTIME: {
                if (peek().type == TokenType::INT_LITERAL) { tc.wtime = std::stoi(consume().value); }
                break;
            }

            case TokenType::BTIME: {
                if (peek().type == TokenType::INT_LITERAL) { tc.btime = std::stoi(consume().value); }
                break;
            }

            case TokenType::WINC: {
                if (peek().type == TokenType::INT_LITERAL) { tc.winc = std::stoi(consume().value); }
                break;
            }
            case TokenType::BINC: {
                if (peek().type == TokenType::INT_LITERAL) { tc.binc = std::stoi(consume().value); }
                break;
            }
        }
    }
    result.timeControl = tc;
    return result;
}

