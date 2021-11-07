cmake_minimum_required(VERSION 3.14)

project(chessengine
	VERSION 1.0
	DESCRIPTION "Chess engine and UI"
	LANGUAGES CXX)

add_executable(chessengine)
target_sources(chessengine PRIVATE
	src/main.cpp
	src/piece.cpp
	src/king.cpp
	src/queen.cpp
	src/rook.cpp
	src/bishop.cpp
	src/knight.cpp
	src/pawn.cpp
	)

############ sioclient

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/lib/src ALL_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/lib/src/internal ALL_SRC)

add_definitions(
	# These will force ASIO to compile without Boost
	-DBOOST_DATE_TIME_NO_LIB
	-DBOOST_REGEX_NO_LIB
	-DASIO_STANDALONE
	# These will force WebsocketPP to compile with C++11
	-D_WEBSOCKETPP_CPP11_STL_
	-D_WEBSOCKETPP_CPP11_FUNCTIONAL_
)

add_library(sioclient ${ALL_SRC})
target_include_directories(sioclient PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/lib/rapidjson/include
	${CMAKE_CURRENT_LIST_DIR}/lib/websocketpp
	${CMAKE_CURRENT_LIST_DIR}/lib/asio/asio/include
	)

############

target_compile_options(chessengine PRIVATE -W4 ${OpenMP_CXX_FLAGS})

set_target_properties(sioclient PROPERTIES COMPILE_FLAGS "-w")

target_include_directories(chessengine PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/include
	${CMAKE_CURRENT_LIST_DIR}/lib
	${CMAKE_CURRENT_LIST_DIR}/lib/src
	)

target_compile_features(chessengine PRIVATE cxx_std_20)

target_compile_definitions(chessengine PRIVATE _WIN32_WINNT=0x0A00)

############ omp

find_package(OpenMP REQUIRED)

############ link libs
target_link_libraries(chessengine PRIVATE ${OpenMP_CXX_FLAGS} sioclient)
