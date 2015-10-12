project(asio_client)
cmake_minimum_required(VERSION 3.0)

#set(Boost_USE_MULTITHREADED      ON)
find_package(Boost 1.50.0 COMPONENTS system thread)

add_definitions(-D_WIN32_WINNT=0x0601)

if (WIN32)
link_directories(${Boost_LIBRARY_DIR})
endif()

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(foo asio_client.cpp)
  target_link_libraries(foo ${Boost_LIBRARIES})
  if(UNIX)
	target_link_libraries(foo pthread)
  endif()
else()
	message("Erro: Boost not found")
endif()

#export CXX=/usr/bin/clang++ 
#export CC=/usr/bin/clang
