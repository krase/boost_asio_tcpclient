project(asio_server)
cmake_minimum_required(VERSION 3.0)

set(Boost_USE_MULTITHREADED      ON)
find_package(Boost 1.50.0 COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Error: Boost not found")
endif()

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    link_directories(${Boost_LIBRARY_DIR})
else()
    add_definitions(-std=c++11)
    add_definitions(-ggdb)
    add_definitions(-O3)
endif()

add_executable(asio_server
    main.cpp
    tcpserver.cpp
    tcpserversession.cpp
)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
if(UNIX)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

