# target
file(GLOB_RECURSE Trader_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Trader_Test_HEADERS "*.hpp")

add_executable(TraderTest 
	${Trader_Test_SOURCES} 
	${Trader_HEADERS}
	${Trader_SOURCES}
)

target_include_directories(TraderTest PUBLIC
	${Arb_INCLUDES}
	${Common_INCLUDES}
	{$Config_INCLUDES}
	${Messaging_INCLUDES}
	${Order_INCLUDES}
	${Service_INCLUDES}
	${Trader_INCLUDES}
)

target_link_libraries(TraderTest 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Arb
	Common
	Config
	Messaging
	Service
	Femas
)

add_test(TraderTest, TraderTest)

add_custom_command(
     TARGET TraderTest
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${CMAKE_COMMAND} -E copy TraderTest UnitTest
     COMMAND TraderTest
)
