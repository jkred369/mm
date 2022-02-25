# target
file(GLOB_RECURSE Messaging_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Messaging_Test_HEADERS "*.hpp")

add_executable(MessagingTest 
	${Messaging_Test_SOURCES} 
	${Messaging_HEADERS}
)

target_include_directories(MessagingTest PUBLIC
	${Common_INCLUDES}
	${Messaging_INCLUDES}
)

target_link_libraries(MessagingTest 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
	Messaging
)

add_test(MessagingTest, MessagingTest)

add_custom_command(
     TARGET MessagingTest
     COMMENT "Run tests"
     POST_BUILD 
#     COMMAND ${CMAKE_COMMAND} -E copy MessagingTest UnitTest
     COMMAND MessagingTest
)