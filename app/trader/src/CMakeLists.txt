# target
file(GLOB_RECURSE Trader_SOURCES "*.cpp")

add_executable(Trader ${Trader_HEADERS} ${Trader_SOURCES})

target_include_directories(Trader PUBLIC
	${Common_INCLUDES}
	${Config_INCLUDES}
	${Messaging_INCLUDES}
	${Threading_INCLUDES}
	${Bus_INCLUDES}
	${Service_INCLUDES}
	${Order_INCLUDES}
	${Femas_INCLUDES}
	${Arb_INCLUDES}
)

target_link_libraries(Trader 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	${FMT_LIBS}
	Common
	Config
	Messaging
	Threading
	Service
	Order
	Femas
	Arb
)

configure_file("../config/simulation.txt" "${CMAKE_CONFIG_OUTPUT_DIRECTORY}/simulation.txt" COPYONLY)