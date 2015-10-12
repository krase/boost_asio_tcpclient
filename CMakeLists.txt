project(asio_client)
cmake_minimum_required(VERSION 3.0)

set(Boost_USE_MULTITHREADED      ON)
find_package(Boost 1.50.0 COMPONENTS system thread)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(foo asio_client.cpp)
  target_link_libraries(foo ${Boost_LIBRARIES})
  target_link_libraries(foo pthread)
else()
	message("Erro: Boost not found")
endif()

#export CXX=/usr/bin/clang++ 
#export CC=/usr/bin/clang
