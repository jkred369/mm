# target
file(GLOB_RECURSE Config_SOURCES "*.cpp")

add_library(Config SHARED ${Config_HEADERS} ${Config_SOURCES})
