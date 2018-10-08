# target
file(GLOB_RECURSE Simulation_SOURCES "*.cpp")

add_library(Simulation SHARED
	${Simulation_HEADERS} 
	${Simulation_SOURCES}
)

target_include_directories(Simulation PUBLIC
	${Bus_INCLUDES}
	${Common_INCLUDES}
	${Messaging_INCLUDES}
	${Service_INCLUDES}
	${Threading_INCLUDES}
	${Product_INCLUDES}
	${Order_INCLUDES}
)

target_link_libraries(Simulation 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
	Messaging
	Service
	Threading
	Product
	Order
)