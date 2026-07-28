cmake_minimum_required(VERSION 4.3)
project(pcdv_minitrading_platform)

set(CMAKE_CXX_STANDARD 20)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
        src/main.cpp
        src/order.cpp
        src/trade.cpp
        src/order_book.cpp
        src/trading_platform.cpp
)

# Header files (for IDE support)
set(HEADERS
        include/trading_platform/order.h
        include/trading_platform/trade.h
        include/trading_platform/order_book.h
        include/trading_platform/trading_platform.h
)

# Create executable
add_executable(trading_platform ${SOURCES} ${HEADERS})
