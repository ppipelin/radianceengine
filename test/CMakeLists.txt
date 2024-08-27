add_executable(perftStart perftStart.cpp ${SOURCES})
add_executable(perftKiwi perftKiwi.cpp ${SOURCES})
add_executable(bitboardMoves bitboardMoves.cpp ${SOURCES})

target_include_directories(perftStart PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(perftKiwi PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(bitboardMoves PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_compile_features(perftStart PRIVATE cxx_std_23)
target_compile_definitions(perftStart PRIVATE _WIN32_WINNT=0x0A00)
target_compile_features(perftKiwi PRIVATE cxx_std_23)
target_compile_definitions(perftKiwi PRIVATE _WIN32_WINNT=0x0A00)
target_compile_features(bitboardMoves PRIVATE cxx_std_23)
target_compile_definitions(bitboardMoves PRIVATE _WIN32_WINNT=0x0A00)
