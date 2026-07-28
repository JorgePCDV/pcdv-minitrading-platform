cmake_minimum_required(VERSION 4.3)
project(pcdv_minitrading_platform)

set(CMAKE_CXX_STANDARD 20)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(pcdv_minitrading_platform main.cpp
        include/trading_platform/order.h
        src/order.cpp
        include/trading_platform/trade.h
        src/trade.cpp)
