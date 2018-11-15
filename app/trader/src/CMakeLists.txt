# target
file(GLOB_RECURSE Trader_SOURCES "*.cpp")

add_executable(Trader ${Trader_HEADERS} ${Trader_SOURCES})

include_directories(
	${CMAKE_SOURCE_DIR}/lib/common/src
	${CMAKE_SOURCE_DIR}/lib/config/src
	${CMAKE_SOURCE_DIR}/lib/messaging/src
	${CMAKE_SOURCE_DIR}/lib/threading/src
	${CMAKE_SOURCE_DIR}/lib/service/src
	${CMAKE_SOURCE_DIR}/lib/order/src
	${CMAKE_SOURCE_DIR}/lib/femas/src
	${CMAKE_SOURCE_DIR}/lib/arb/src
)

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

configure_file("../config/simulation.cfg" "${CMAKE_CONFIG_OUTPUT_DIRECTORY}/simulation.cfg" COPYONLY)
configure_file("../config/productDownload.cfg" "${CMAKE_CONFIG_OUTPUT_DIRECTORY}/productDownload.cfg" COPYONLY)
configure_file("../config/backtest.cfg" "${CMAKE_CONFIG_OUTPUT_DIRECTORY}/backtest.cfg" COPYONLY)
