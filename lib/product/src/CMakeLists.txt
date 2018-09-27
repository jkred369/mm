# target
file(GLOB_RECURSE Product_SOURCES "*.cpp")

add_library(Product SHARED ${Product_SOURCES})

target_include_directories(Product PUBLIC
	${Common_INCLUDES}
	${Messaging_INCLUDES}
	${Product_INCLUDES}
)

target_link_libraries(Product
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
	Messaging
)