# target
file(GLOB_RECURSE System_Test_SOURCES "*.cpp")
file(GLOB_RECURSE System_Test_HEADERS "*.hpp")

add_executable(Test 
	${System_Test_SOURCES} 
	${System_HEADERS}
)

target_include_directories(Test PUBLIC
	${Common_INCLUDES}
	${System_INCLUDES}
)

target_link_libraries(Test 
	${Boost_LIBRARIES}
	${POCO_LIBS}
	${GTEST_LIBS}
	${Common}
)

add_test(SystemTest, Test)

add_custom_command(
     TARGET Test
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${CMAKE_COMMAND} -E copy Test UnitTest
     COMMAND Test
)