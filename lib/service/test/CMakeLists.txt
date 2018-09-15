# target
file(GLOB_RECURSE Service_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Service_Test_HEADERS "*.hpp")

add_executable(ServiceTest 
	${Service_Test_SOURCES} 
	${Service_HEADERS}
)

target_include_directories(ServiceTest PUBLIC
	${Service_INCLUDES}
)

target_link_libraries(ServiceTest 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	${Common}
)

add_test(ServiceTest, ServiceTest)

add_custom_command(
     TARGET ServiceTest
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${CMAKE_COMMAND} -E copy ServiceTest UnitTest
     COMMAND ServiceTest
)