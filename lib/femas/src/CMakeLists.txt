# target
file(GLOB_RECURSE Femas_SOURCES "*.cpp")

add_library(Femas SHARED
	${Femas_HEADERS} 
	${Femas_SOURCES}
)

target_include_directories(Femas PUBLIC
	${FEMAS_INCLUDE_DIR}
	${Bus_INCLUDES}
	${Common_INCLUDES}
	${Messaging_INCLUDES}
	${Service_INCLUDES}
	${Threading_INCLUDES}
	${Product_INCLUDES}
	${Order_INCLUDES}
)

target_link_libraries(Femas 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	${FEMAS_LIBS}
	Common
	Messaging
	Service
	Threading
	Product
	Order
)