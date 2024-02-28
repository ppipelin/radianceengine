create_test_sourcelist (Tests
	CommonCppTests.cpp
	perftStart.cpp
	perftKiwi.cpp
)

add_executable (CommonCppTests ${Tests})

target_compile_features(CommonCppTests PRIVATE cxx_std_23)
target_sources(CommonCppTests PRIVATE
	../src/piece.cpp
	../src/king.cpp
	../src/queen.cpp
	../src/rook.cpp
	../src/bishop.cpp
	../src/knight.cpp
	../src/pawn.cpp
	)

target_include_directories(CommonCppTests PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/../include
	)


# remove the test driver source file
set (TestsToRun ${Tests})
remove (TestsToRun CommonCppTests.cpp)

# Add all the ADD_TEST for each test
foreach (test ${TestsToRun})
  get_filename_component (TName ${test} NAME_WE)
  add_test (NAME ${TName} COMMAND CommonCppTests ${TName})
endforeach ()
