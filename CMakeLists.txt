cmake_minimum_required(VERSION 3.0)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
find_package(GSL REQUIRED)
find_package(PNG REQUIRED)

include_directories(${GSL_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS})
add_definitions(${PNG_DEFINITIONS})
add_executable(time-series time-series.cpp lanczos.cpp image.cpp colormap.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

target_link_libraries(time-series ${GSL_LIBRARIES} ${PNG_LIBRARIES})
