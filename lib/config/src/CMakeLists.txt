# target
file(GLOB_RECURSE Config_SOURCES "*.cpp")

add_library(Config SHARED ${Config_HEADERS} ${Config_SOURCES})

target_include_directories(Config PUBLIC
	${Common_INCLUDES}
)

target_link_libraries(Config 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
)