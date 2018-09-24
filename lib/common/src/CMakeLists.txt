# target
file(GLOB_RECURSE Common_SOURCES "*.cpp")

add_library(Common SHARED ${Common_HEADERS} ${Common_SOURCES})

target_include_directories(Common PUBLIC
	${SPDLOG_INCLUDE_DIR}
)

target_link_libraries(Common 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	${FMT_LIBS}
)