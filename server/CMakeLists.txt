project(server)
cmake_minimum_required(VERSION 3.0)

set(Boost_USE_MULTITHREADED      ON)
find_package(Boost 1.50.0 COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})

add_definitions(-D_WIN32_WINNT=0x0601)
add_definitions(-std=c++11)
add_definitions(-ggdb)
#add_definitions(-O3)

if (WIN32)
  link_directories(${Boost_LIBRARY_DIR})
endif()

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
if(UNIX)
  target_link_libraries(${PROJECT_NAME} pthread)
endif()

