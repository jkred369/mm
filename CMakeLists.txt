cmake_minimum_required(VERSION 2.6)
project(MM)

# define libraries
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
 
find_package(Boost 1.58.0 COMPONENTS filesystem regex) 
include_directories(${Boost_INCLUDE_DIRS})

find_package(Poco REQUIRED Foundation Util Net)
include_directories(${POCO_INCLUDE_DIR})

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# compiler options
set(CMAKE_CXX_FLAGS "-g -Wall -O2 -pipe -std=c++11")

# project structure
add_subdirectory(lib)