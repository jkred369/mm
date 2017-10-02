# project structure
set(Common_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/common/src")
file(GLOB_RECURSE Common_HEADERS "${Common_INCLUDES}/*.hpp")

add_subdirectory(common)

set(System_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/system/src")
file(GLOB_RECURSE System_HEADERS "${System_INCLUDES}/*.hpp")

add_subdirectory(system)