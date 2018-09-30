# target
file(GLOB_RECURSE Femas_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Femas_Test_HEADERS "*.hpp")

add_executable(FemasTest 
	${Femas_Test_SOURCES} 
	${Femas_HEADERS}
)

target_include_directories(FemasTest PUBLIC
	${Femas_INCLUDES}
)

target_link_libraries(FemasTest 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Femas
)

add_test(FemasTest, FemasTest)

add_custom_command(
     TARGET FemasTest
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${CMAKE_COMMAND} -E copy FemasTest UnitTest
     COMMAND FemasTest
)