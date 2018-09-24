# target
file(GLOB_RECURSE Order_SOURCES "*.cpp")

add_library(Order SHARED
	${Order_HEADERS} 
	${Order_SOURCES}
)

target_include_directories(Order PUBLIC
	${Bus_INCLUDES}
	${Common_INCLUDES}
	${Messaging_INCLUDES}
)

target_link_libraries(Order 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
	Messaging
)