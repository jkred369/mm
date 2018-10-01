# target
file(GLOB_RECURSE Arb_SOURCES "*.cpp")

add_library(Arb SHARED ${Arb_HEADERS} ${Arb_SOURCES})

target_include_directories(Arb PUBLIC
	${Common_INCLUDES}
	${Config_INCLUDES}
	${Messaging_INCLUDES}
	${Bus_INCLUDES}
	${Threading_INCLUDES}
	${Service_INCLUDES}
)

target_link_libraries(Arb 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
	Config
	Messaging
	Threading
	Service
)