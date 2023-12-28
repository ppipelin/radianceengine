cmake_minimum_required(VERSION 3.14)

project(chessengine
	VERSION 1.0
	DESCRIPTION "Chess engine and UI"
	LANGUAGES CXX)

add_executable(chessengine)
target_sources(chessengine PRIVATE
	src/main.cpp
	src/uci.cpp
	src/ucioption.cpp
	src/piece.cpp
	src/king.cpp
	src/queen.cpp
	src/rook.cpp
	src/bishop.cpp
	src/knight.cpp
	src/pawn.cpp
	)

# target_compile_options(chessengine PRIVATE -W4 ${OpenMP_CXX_FLAGS})
target_compile_options(chessengine PRIVATE -W4)

target_include_directories(chessengine PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/include
	${CMAKE_CURRENT_LIST_DIR}/lib
	${CMAKE_CURRENT_LIST_DIR}/lib/src
	)

target_compile_features(chessengine PRIVATE cxx_std_23)

target_compile_definitions(chessengine PRIVATE _WIN32_WINNT=0x0A00)

############ omp

# find_package(OpenMP REQUIRED)

############ link libs
# target_link_libraries(chessengine PRIVATE ${OpenMP_CXX_FLAGS})
