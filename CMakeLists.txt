cmake_minimum_required(VERSION 3.24)

include(CheckCXXCompilerFlag)

project(radiance
	VERSION 1.0
	DESCRIPTION "Radiance chess engine"
	LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

if(MSVC)
	add_compile_options(/W4)

	check_cxx_compiler_flag("/GL" COMPILER_SUPPORTS_GL)
	check_cxx_compiler_flag("/Gv" COMPILER_SUPPORTS_Gv)
	check_cxx_compiler_flag("/arch:SSE2" COMPILER_SUPPORTS_SSE2)
	check_cxx_compiler_flag("/arch:SSE4.1" COMPILER_SUPPORTS_SSE41)
	check_cxx_compiler_flag("/arch:SSE4.2" COMPILER_SUPPORTS_SSE42)
	check_cxx_compiler_flag("/arch:AVX" COMPILER_SUPPORTS_AVX)
	check_cxx_compiler_flag("/arch:AVX2" COMPILER_SUPPORTS_AVX2)
	check_cxx_compiler_flag("/arch:AVX512" COMPILER_SUPPORTS_AVX512)
	check_cxx_compiler_flag("/arch:AVX10.1" COMPILER_SUPPORTS_AVX101)
	check_cxx_compiler_flag("/O2" COMPILER_SUPPORTS_O2)
	check_cxx_compiler_flag("/Ob" COMPILER_SUPPORTS_OB)
	check_cxx_compiler_flag("/Oi" COMPILER_SUPPORTS_Oi)
	check_cxx_compiler_flag("/Ox" COMPILER_SUPPORTS_OX)
	check_cxx_compiler_flag("/Oy" COMPILER_SUPPORTS_OY)
	check_cxx_compiler_flag("/MT" COMPILER_SUPPORTS_MT)
	check_cxx_compiler_flag("/MP" COMPILER_SUPPORTS_MP)
	check_cxx_compiler_flag("/fp:fast" COMPILER_SUPPORTS_FPF)

	if(COMPILER_SUPPORTS_GL)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/GL>")
		target_link_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/LTCG>")
	endif()

	if(COMPILER_SUPPORTS_Gv)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/Gv>")
	endif()

	if(COMPILER_SUPPORTS_SSE2)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/arch:SSE2>")
	endif()

	if(COMPILER_SUPPORTS_SSE41)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/arch:SSE4.1>")
	endif()

	if(COMPILER_SUPPORTS_SSE42)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/arch:SSE4.2>")
	endif()

	if(COMPILER_SUPPORTS_AVX)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/arch:AVX>")
	endif()

	if(COMPILER_SUPPORTS_AVX2)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/arch:AVX2>")
	endif()

	if(COMPILER_SUPPORTS_AVX512)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/arch:AVX512>")
	endif()

	if(COMPILER_SUPPORTS_AVX101)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/arch:AVX10.1>")
	endif()

	if(COMPILER_SUPPORTS_O2)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/O2>")
	endif()

	if(COMPILER_SUPPORTS_OB)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/Ob>")
	endif()

	if(COMPILER_SUPPORTS_OI)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/Oi>")
	endif()

	if(COMPILER_SUPPORTS_OX)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/Ox>")
	endif()

	if(COMPILER_SUPPORTS_OY)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/Oy>")
	endif()

	if(COMPILER_SUPPORTS_MT)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/MT>")
	endif()

	if(COMPILER_SUPPORTS_MP)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/MP>")
	endif()

	if(COMPILER_SUPPORTS_FPF)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:/fp:fast>")
	endif()

else()
	# GCC and Clang specific options
	target_compile_options(radiance PRIVATE -Wall -Wextra -Wpedantic -Wno-unknown-pragmas -Wno-ignored-qualifiers)

	check_cxx_compiler_flag("-Ofast" COMPILER_SUPPORTS_OFAST)
	check_cxx_compiler_flag("-flto" COMPILER_SUPPORTS_LTO)
	check_cxx_compiler_flag("-ftree-vectorize" COMPILER_SUPPORTS_TREE_VECTORIZE)
	check_cxx_compiler_flag("-funroll-loops" COMPILER_SUPPORTS_UNROLL_LOOPS)
	check_cxx_compiler_flag("-static" COMPILER_SUPPORTS_STATIC)
	check_cxx_compiler_flag("-static-libgcc" COMPILER_SUPPORTS_STATIC_LIBGCC)
	check_cxx_compiler_flag("-static-libstdc++" COMPILER_SUPPORTS_STATIC_LIBCPP)
	check_cxx_compiler_flag("-finline-functions" COMPILER_SUPPORTS_INLINE)
	check_cxx_compiler_flag("-pipe" COMPILER_SUPPORTS_PIPE)
	check_cxx_compiler_flag("-fstrict-aliasing" COMPILER_SUPPORTS_STRICT_ALIASING)
	check_cxx_compiler_flag("-fomit-frame-pointer" COMPILER_SUPPORTS_OMIT_FRAME_POINTER)
	check_cxx_compiler_flag("-lm" COMPILER_SUPPORTS_LM)
	check_cxx_compiler_flag("-fwhole-program" COMPILER_SUPPORTS_WHOLE_PROGRAM)
	check_cxx_compiler_flag("-fipa-pta" COMPILER_SUPPORTS_IPA_PTA)

	check_cxx_compiler_flag("-mpopcnt" COMPILER_SUPPORTS_POPCNT)
	check_cxx_compiler_flag("-msse3" COMPILER_SUPPORTS_SSE3)
	check_cxx_compiler_flag("-mssse3" COMPILER_SUPPORTS_SSSE3)
	check_cxx_compiler_flag("-msse4.1" COMPILER_SUPPORTS_SSE41)
	check_cxx_compiler_flag("-msse4.2" COMPILER_SUPPORTS_SSE42)
	check_cxx_compiler_flag("-mfma" COMPILER_SUPPORTS_FMA)
	check_cxx_compiler_flag("-mbmi" COMPILER_SUPPORTS_BMI)
	check_cxx_compiler_flag("-mbmi2" COMPILER_SUPPORTS_BMI2)
	check_cxx_compiler_flag("-mavx" COMPILER_SUPPORTS_AVX)
	check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
	check_cxx_compiler_flag("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
	check_cxx_compiler_flag("-mtune=native" COMPILER_SUPPORTS_MTUNE_NATIVE)

	if(COMPILER_SUPPORTS_OFAST)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-Ofast>")
	endif()

	if(COMPILER_SUPPORTS_LTO)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-flto>")
	endif()

	if(COMPILER_SUPPORTS_TREE_VECTORIZE)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-ftree-vectorize>")
	endif()

	if(COMPILER_SUPPORTS_STATIC)
		target_link_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-static>")
	endif()

	if(COMPILER_SUPPORTS_STATIC_LIBGCC)
		target_link_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-static-libgcc>")
	endif()

	if(COMPILER_SUPPORTS_STATIC_LIBCPP)
		target_link_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-static-libstdc++>")
	endif()

	if(COMPILER_SUPPORTS_INLINE)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-finline-functions>")
	endif()

	if(COMPILER_SUPPORTS_PIPE)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-pipe>")
	endif()

	if(COMPILER_SUPPORTS_UNROLL_LOOPS)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-funroll-loops>")
	endif()

	if(COMPILER_SUPPORTS_STRICT_ALIASING)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-fstrict-aliasing>")
	endif()

	if(COMPILER_SUPPORTS_OMIT_FRAME_POINTER)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-fomit-frame-pointer>")
	endif()

	if(COMPILER_SUPPORTS_LM)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-lm>")
	endif()

	if(COMPILER_SUPPORTS_WHOLE_PROGRAM)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-fwhole-program>")
	endif()

	if(COMPILER_SUPPORTS_IPA_PTA)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-fipa-pta>")
	endif()

	if(COMPILER_SUPPORTS_SSE3)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-mpopcnt>")
	endif()

	if(COMPILER_SUPPORTS_SSSE3)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-msse3>")
	endif()

	if(COMPILER_SUPPORTS_POPCNT)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-mssse3>")
	endif()

	if(COMPILER_SUPPORTS_SSE41)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-msse4.1>")
	endif()

	if(COMPILER_SUPPORTS_SSE42)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-msse4.2>")
	endif()

	if(COMPILER_SUPPORTS_FMA)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-mfma>")
	endif()

	if(COMPILER_SUPPORTS_BMI)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-mbmi>")
	endif()

	if(COMPILER_SUPPORTS_BMI2)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-mbmi2>")
	endif()

	if(COMPILER_SUPPORTS_AVX)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-mavx>")
	endif()

	if(COMPILER_SUPPORTS_AVX2)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-mavx2>")
	endif()

	if(COMPILER_SUPPORTS_MARCH_NATIVE)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-march=native>")
	endif()

	if(COMPILER_SUPPORTS_MTUNE_NATIVE)
		target_compile_options(radiance PRIVATE "$<$<CONFIG:RELEASE>:-mtune=native>")
	endif()
endif()

target_compile_features(radiance PRIVATE cxx_std_23)

target_include_directories(radiance PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/include
)

# Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
	add_subdirectory(test)
endif()

add_test(NAME perftStart COMMAND perftStart)
add_test(NAME perftKiwi COMMAND perftKiwi)
add_test(NAME bitboardMoves COMMAND bitboardMoves)
