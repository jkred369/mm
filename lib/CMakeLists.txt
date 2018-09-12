# project structure
set(Common_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/common/src")
file(GLOB_RECURSE Common_HEADERS "${Common_INCLUDES}/*.hpp")

add_subdirectory(common)

set(Messaging_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/messaging/src")
file(GLOB_RECURSE Threading_HEADERS "${Messaging_INCLUDES}/*.hpp")

add_subdirectory(messaging)

set(Threading_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/threading/src")
file(GLOB_RECURSE Threading_HEADERS "${Threading_INCLUDES}/*.hpp")

add_subdirectory(threading)

set(Femas_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/threading/src")
file(GLOB_RECURSE Femas_HEADERS "${Femas_INCLUDES}/*.hpp")

add_subdirectory(femas)