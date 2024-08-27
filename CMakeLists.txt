cmake_minimum_required(VERSION 3.24)

include(CheckCXXCompilerFlag)

project(radiance
	VERSION 1.0
	DESCRIPTION "Radiance chess engine"
	LANGUAGES CXX
)

add_executable(radiance)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCES
	${SOURCE_DIR}/uci.cpp
	${SOURCE_DIR}/ucioption.cpp
	${SOURCE_DIR}/piece.cpp
	${SOURCE_DIR}/king.cpp
	${SOURCE_DIR}/queen.cpp
	${SOURCE_DIR}/rook.cpp
	${SOURCE_DIR}/bishop.cpp
	${SOURCE_DIR}/knight.cpp
	${SOURCE_DIR}/pawn.cpp
)

target_sources(radiance PRIVATE
	src/main.cpp
	${SOURCES}
)

set_target_properties(radiance PROPERTIES LINK_WHAT_YOU_USE TRUE)

# target_compile_options(radiance PRIVATE -W4 ${OpenMP_CXX_FLAGS})
if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Wno-unknown-pragmas)
endif()

target_include_directories(radiance PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/include
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

check_cxx_compiler_flag("/arch:AVX2" COMPILER_SUPPORTS_AVX2_WIN)
check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
check_cxx_compiler_flag("-mbmi2" COMPILER_SUPPORTS_BMI2)

if(COMPILER_SUPPORTS_AVX2_WIN)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
endif()

if(COMPILER_SUPPORTS_AVX2)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
endif()

if(COMPILER_SUPPORTS_BMI2)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbmi2")
endif()

target_compile_features(radiance PRIVATE cxx_std_23)

target_compile_definitions(radiance PRIVATE _WIN32_WINNT=0x0A00)

# OMP
# find_package(OpenMP REQUIRED)

# Link libraries
# target_link_libraries(radiance PRIVATE ${OpenMP_CXX_FLAGS})

# Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
	add_subdirectory(test)
endif()

# Define tests
add_test(NAME perftStart COMMAND perftStart)
add_test(NAME perftKiwi COMMAND perftKiwi)
add_test(NAME bitboardMoves COMMAND bitboardMoves)
