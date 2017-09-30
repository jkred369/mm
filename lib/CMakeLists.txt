# project structure
set(Common_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/common/src")
file(GLOB_RECURSE Common_HEADERS "${Common_INCLUDES}/*.hpp")

add_subdirectory(common)