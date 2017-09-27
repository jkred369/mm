# target
file(GLOB_RECURSE Common_SOURCES "*.cpp")
file(GLOB_RECURSE Common_HEADERS "*.hpp")

add_library(Common ${Common_SOURCES})