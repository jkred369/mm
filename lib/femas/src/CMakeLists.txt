# target
file(GLOB_RECURSE Femas_SOURCES "*.cpp")

add_library(Femas SHARED
	${Femas_HEADERS} 
	${Femas_SOURCES}
)

target_include_directories(Femas PUBLIC
	${Common_INCLUDES}
)

target_link_libraries(Femas 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
)