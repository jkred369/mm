# target
file(GLOB_RECURSE Common_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Common_Test_HEADERS "*.hpp")

add_executable(CommonTest 
	${Common_Test_SOURCES} 
	${Common_HEADERS}
)

target_include_directories(CommonTest PUBLIC
	${Common_INCLUDES}
)

target_link_libraries(CommonTest 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
)

add_test(CommonTest, CommonTest)

add_custom_command(
     TARGET CommonTest
     COMMENT "Run tests"
     POST_BUILD 
#     COMMAND ${CMAKE_COMMAND} -E copy CommonTest UnitTest
     COMMAND CommonTest
)