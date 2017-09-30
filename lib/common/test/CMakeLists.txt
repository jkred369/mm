# target
file(GLOB_RECURSE Common_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Common_Test_HEADERS "*.hpp")

add_executable(Test 
	${Common_Test_SOURCES} 
	${Common_HEADERS}
)

target_include_directories(Test PUBLIC
	${Common_INCLUDES}
)

target_link_libraries(Test 
	${Boost_LIBRARIES}
	${POCO_LIBS}
	${GTEST_LIBS}
)

add_test(CommonTest, Test)

add_custom_command(
     TARGET Test
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND Test
)