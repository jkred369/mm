# target
file(GLOB_RECURSE Common_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Common_Test_HEADERS "*.hpp")

add_executable(Test ${Common_Test_SOURCES})

target_link_libraries(Test 
	${Boost_LIBRARIES}
	${POCO_LIBRARIES}
	${GTest_LIBRARIES}
)

add_test(CommonTest, Test)