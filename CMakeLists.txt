cmake_minimum_required(VERSION 3.15)
project(search_engine)

set(CMAKE_CXX_STANDARD 14)

add_executable(search_engine server/run/testISRSpan.cpp utility/src/mString.cpp)