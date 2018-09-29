# target
file(GLOB_RECURSE Bus_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Bus_Test_HEADERS "*.hpp")

add_executable(BusTest 
	${Bus_Test_SOURCES}
)

target_include_directories(BusTest PUBLIC
	${Bus_INCLUDES}
	${Common_INCLUDES}
	${Messaging_INCLUDES}
	${Threading_INCLUDES}
)

target_link_libraries(BusTest 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
	Messaging
	Threading
)

add_test(BusTest, BusTest)

add_custom_command(
     TARGET BusTest
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${CMAKE_COMMAND} -E copy BusTest UnitTest
     COMMAND BusTest
)