# target
file(GLOB_RECURSE Product_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Product_Test_HEADERS "*.hpp")

add_executable(ProductTest 
	${Product_Test_SOURCES} 
	${Product_HEADERS}
)

target_include_directories(ProductTest PUBLIC
	${Common_INCLUDES}
	${Messaging_INCLUDES}
	${Product_INCLUDES}
)

target_link_libraries(ProductTest 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
	Messaging
)

add_test(ProductTest, ProductTest)

add_custom_command(
     TARGET ProductTest
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${CMAKE_COMMAND} -E copy ProductTest UnitTest
     COMMAND ProductTest
)