# target
file(GLOB_RECURSE Bus_SOURCES "*.cpp")

# add_library(Bus SHARED ${Bus_HEADERS} ${Bus_SOURCES})

#target_include_directories(Bus PUBLIC
#	${Common_INCLUDES}
#	${Threading_INCLUDES}
#)

#target_link_libraries(Bus
#	${Boost_LIBRARIES}
#	${TBB_LIBS}
#	${POCO_LIBS}
#	${GTEST_LIBS}
#	Common
#	Threading
#)