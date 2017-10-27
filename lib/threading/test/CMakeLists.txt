# target
file(GLOB_RECURSE Threading_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Threading_Test_HEADERS "*.hpp")

add_executable(ThreadingTest 
	${Threading_Test_SOURCES} 
	${Threading_HEADERS}
)

target_include_directories(ThreadingTest PUBLIC
	${Common_INCLUDES}
	${Threading_INCLUDES}
)

target_link_libraries(ThreadingTest 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	${Common}
)

add_test(ThreadingTest, ThreadingTest)

add_custom_command(
     TARGET ThreadingTest
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${CMAKE_COMMAND} -E copy ThreadingTest UnitTest
     COMMAND ThreadingTest
)