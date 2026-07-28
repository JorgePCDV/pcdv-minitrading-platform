cmake_minimum_required(VERSION 4.3)
project(pcdv_minitrading_platform)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(LIB_SOURCES
        src/order.cpp
        src/trade.cpp
        src/order_book.cpp
        src/trading_platform.cpp
)

set(HEADERS
        include/trading_platform/order.h
        include/trading_platform/trade.h
        include/trading_platform/order_book.h
        include/trading_platform/trading_platform.h
)

add_executable(trading_platform src/main.cpp ${LIB_SOURCES} ${HEADERS})

option(BUILD_TESTS "Build unit tests" ON)

if (BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)

    set(TEST_SOURCES
            tests/test_order.cpp
            tests/test_trade.cpp
    )

    add_executable(trading_platform_tests ${TEST_SOURCES} ${LIB_SOURCES} ${HEADERS})

    target_link_libraries(trading_platform_tests
            PRIVATE gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(trading_platform_tests)

    if (MSVC)
        target_compile_options(trading_platform_tests PRIVATE /W4)
    else ()
        target_compile_options(trading_platform_tests PRIVATE -Wall -Wextra -pthread)
    endif ()

    message(STATUS "Tests enabled - run 'make test' or build 'trading_platform_tests'")
endif ()