cmake_minimum_required(VERSION 3.14.0)

project(chessengine
 VERSION 1.0
 DESCRIPTION "Chess engine and UI"
 LANGUAGES CXX)

set(PROJECT "D:/Documents/dev/cpp/chessengine" PATH)
set(SOURCES src/hello.cpp )# include/include.h include/piece.h include/pawn.h include/board.h)

add_executable(chessengine ${SOURCES})

target_include_directories(chessengine PRIVATE ${PROJECT}/include)
# target_include_directories(chessengine PRIVATE ${PROJECT}/src)

target_compile_features(chessengine PRIVATE cxx_std_20)

# target_compile_definitions(chessengine PRIVATE BOARD)

find_package(OpenMP REQUIRED)

target_compile_options(chessengine PRIVATE -W4 ${OpenMP_CXX_FLAGS})
target_link_libraries(chessengine PRIVATE ${OpenMP_CXX_FLAGS})