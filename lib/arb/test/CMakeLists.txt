# target
file(GLOB_RECURSE Arb_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Arb_Test_HEADERS "*.hpp")

add_executable(ArbTest 
	${Arb_Test_SOURCES} 
	${Arb_HEADERS}
)

target_include_directories(ArbTest PUBLIC
	${Arb_INCLUDES}
	${Common_INCLUDES}
	{$Config_INCLUDES}
	${Messaging_INCLUDES}
	${Order_INCLUDES}
	${Service_INCLUDES}
)

target_link_libraries(ArbTest 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	${Arb}
	${Common}
	${Config}
	${Messaging}
	${Order}
	${Service}
)

add_test(ArbTest, ArbTest)

add_custom_command(
     TARGET ArbTest
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${CMAKE_COMMAND} -E copy ArbTest UnitTest
     COMMAND ArbTest
)