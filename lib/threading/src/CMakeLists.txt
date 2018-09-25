# target
file(GLOB_RECURSE Threading_SOURCES "*.cpp")

add_library(Threading SHARED ${Threading_SOURCES})

target_include_directories(Threading PUBLIC
	${Common_INCLUDES}
)

target_link_libraries(Threading
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
)