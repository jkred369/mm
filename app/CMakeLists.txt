# project structure
set(Trader_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/trader/src")
file(GLOB_RECURSE Trader_HEADERS "${Trader_INCLUDES}/*.hpp")
add_subdirectory(trader)
