# target
file(GLOB_RECURSE Simulation_Test_SOURCES "*.cpp")
file(GLOB_RECURSE Simulation_Test_HEADERS "*.hpp")

add_executable(SimulationTest 
	${Simulation_Test_SOURCES} 
	${Simulation_HEADERS}
)

target_include_directories(SimulationTest PUBLIC
	${Common_INCLUDES}
	${Simulation_INCLUDES}
)

target_link_libraries(SimulationTest 
	${Boost_LIBRARIES}
	${TBB_LIBS}
	${POCO_LIBS}
	${GTEST_LIBS}
	Common
	Simulation
)

add_test(SimulationTest, SimulationTest)

add_custom_command(
     TARGET SimulationTest
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${CMAKE_COMMAND} -E copy SimulationTest UnitTest
     COMMAND SimulationTest
)