# target
file(GLOB_RECURSE Messaging_SOURCES "*.cpp")

add_library(Messaging SHARED
	${Messaging_HEADERS} 
	${Messaging_SOURCES}
)

target_include_directories(Messaging PUBLIC
	${Common_INCLUDES}
)

target_link_libraries(Messaging 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
)