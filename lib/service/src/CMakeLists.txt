# target
file(GLOB_RECURSE Service_SOURCES "*.cpp")

add_library(Service SHARED ${Service_SOURCES})

target_include_directories(Service PUBLIC
	${Bus_INCLUDES}
	${Common_INCLUDES}
	${Config_INCLUDES}
	${Messaging_INCLUDES}
	${Threading_INCLUDES}
)

target_link_libraries(Service 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
	Config
	Messaging
)