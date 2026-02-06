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
)
add_subdirectory(tests)

add_executable(OnyxCPP main.cpp)
target_link_libraries(OnyxCPP OnyxCoreLib)
