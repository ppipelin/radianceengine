cmake_minimum_required(VERSION 3.14.0)

project(chessengine
 VERSION 1.0
 DESCRIPTION "A brief CMake experiment"
 LANGUAGES CXX)

set(PROJECT "D:/Documents/dev/cpp/chessengine" PATH)
set(SOURCES src/hello.cpp include/include.h include/piece.h include/pawn.h include/board.h)

add_executable(chessengine ${SOURCES})

target_include_directories(chessengine PRIVATE ${PROJECT}/include)
# target_include_directories(chessengine PRIVATE ${PROJECT}/src)

target_compile_features(chessengine PRIVATE cxx_std_20)

# target_compile_definitions(chessengine PRIVATE BOARD)

# find_package(OpenMP)
# if (OPENMP_FOUND)
#  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif()
