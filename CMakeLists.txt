cmake_minimum_required(VERSION 4.0)
project(OnyxCPP)

set(CMAKE_CXX_STANDARD 23)

add_library(OnyxCoreLib
        include/Move.h
        src/Board.cpp
        include/Board.h
        include/types.h
        include/utils.h
        include/Fen.h
        src/MoveGenerator.cpp
        include/MoveGenerator.h
        src/MagicBitboards.cpp
        include/MagicBitboards.h
        src/Piece.cpp
        include/MoveList.h
        include/PerftRunner.h
        src/PerftRunner.cpp
        src/Referee.cpp
        include/Referee.h
        src/UciTokeniser.cpp
        include/UciTokeniser.h
        src/UciParser.cpp
        include/UciParser.h
        include/CLIBot.h
        src/CLIBot.cpp
        src/SearchWorker.cpp
        include/SearchWorker.h
        include/TimeControl.h
        src/Evaluator.cpp
        include/Evaluator.h
        include/Timer.h
        src/SearchController.cpp
        include/SearchController.h
)
add_subdirectory(tests)
target_include_directories(OnyxCoreLib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(OnyxCPP main.cpp)
target_link_libraries(OnyxCPP OnyxCoreLib)
