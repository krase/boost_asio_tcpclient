project(asio_client)
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

set(target test_client)

if(Boost_FOUND)
#		add_executable(${target}
#		asio_client.cpp
#		test.cpp
#	)
#	target_link_libraries(${target} ${Boost_LIBRARIES})
	add_executable(${target}_nc
			asio_client.cpp
			can_net_comm.cpp
			frame.cpp
	)
	target_link_libraries(${target}_nc ${Boost_LIBRARIES})
	if(UNIX)
#			target_link_libraries(${target} pthread)
			target_link_libraries(${target}_nc pthread)
	endif()
else()
	message("Erro: Boost not found")
endif()

#export CXX=/usr/bin/clang++ 
#export CC=/usr/bin/clang
