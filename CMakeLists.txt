cmake_minimum_required(VERSION 2.6)
project(MM)

# define libraries
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
 
find_package(Boost 1.58.0 COMPONENTS filesystem regex) 
include_directories(${Boost_INCLUDE_DIRS})

set(TBB_INCLUDE_DIR "/usr/include/tbb-2018.1")
set(TBB_LIB_DIR "/usr/lib/tbb-2018.1/lib")
find_library(TBB_LIBS tbb HINTS "${TBB_LIB_DIR}")

include_directories(${TBB_INCLUDE_DIR})

set(POCO_INCLUDE_DIR "/usr/include/poco-1.7.9")
set(POCO_LIB_DIR "/usr/lib/poco-1.7.9")
set(POCO_LIBS
	"${POCO_LIB_DIR}/libPocoNet.so"
	"${POCO_LIB_DIR}/libPocoUtil.so"
	"${POCO_LIB_DIR}/libPocoFoundation.so")

include_directories(${POCO_INCLUDE_DIR})

set(GTEST_INCLUDE_DIR "/usr/include/gtest")
set(GTEST_LIB_DIR "/usr/lib/gtest-1.8.0")
set(GTEST_LIBS
	"${GTEST_LIB_DIR}/libgtest.a"
	"${GTEST_LIB_DIR}/libgtest_main.a"
	"${GTEST_LIB_DIR}/libgmock.a"
	"${GTEST_LIB_DIR}/libgmock_main.a"
)

include_directories(${GTEST_INCLUDE_DIR})

set(FEMAS_INCLUDE_DIR "/usr/include/femas")
set(FEMAS_LIB_DIR "/usr/lib/femas")
set(FEMAS_LIBS
	"${FEMAS_LIB_DIR}/libUSTPmduserapi.so"
	"${FEMAS_LIB_DIR}/libUSTPtraderapi.so"
)

include_directories(${FEMAS_INCLUDE_DIR})

# compiler options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -Wall -O2 -pipe -pthread -std=c++14")

# testing
set(BUILD_TESTING ON)

# project structure
add_subdirectory(lib)
