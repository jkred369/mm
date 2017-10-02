# target
file(GLOB_RECURSE System_Test_SOURCES "*.cpp")
file(GLOB_RECURSE System_Test_HEADERS "*.hpp")

add_executable(SystemTest 
	${System_Test_SOURCES} 
	${System_HEADERS}
)

target_include_directories(SystemTest PUBLIC
	${Common_INCLUDES}
	${System_INCLUDES}
)

target_link_libraries(SystemTest 
	${Boost_LIBRARIES}
	${POCO_LIBS}
	${GTEST_LIBS}
	${Common}
)

add_test(SystemTest, SystemTest)

add_custom_command(
     TARGET SystemTest
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${CMAKE_COMMAND} -E copy SystemTest UnitTest
     COMMAND SystemTest
)