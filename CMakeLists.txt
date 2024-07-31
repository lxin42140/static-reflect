cmake_minimum_required(VERSION 3.21)
project(static_reflect)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)
include_directories(nlohmann)
include_directories(reflect)

file(GLOB_RECURSE CMAKE_FILES *.cpp *.h *.hpp *.cc)
add_executable(static_reflect ${CMAKE_FILES})
