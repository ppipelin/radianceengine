cmake_minimum_required(VERSION 3.14)

project(radiance
	VERSION 1.0
	DESCRIPTION "Chess engine and UI"
	LANGUAGES CXX)

add_executable(radiance)
target_sources(radiance PRIVATE
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

# target_compile_options(radiance PRIVATE -W4 ${OpenMP_CXX_FLAGS})
target_compile_options(radiance PRIVATE -W4)

target_include_directories(radiance PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/include
	)

target_compile_features(radiance PRIVATE cxx_std_23)

target_compile_definitions(radiance PRIVATE _WIN32_WINNT=0x0A00)

enable_testing()

# add_test(NAME perft COMMAND radiance.exe "go perft 4")

add_subdirectory(test)

############ omp

# find_package(OpenMP REQUIRED)

############ link libs
# target_link_libraries(radiance PRIVATE ${OpenMP_CXX_FLAGS})
