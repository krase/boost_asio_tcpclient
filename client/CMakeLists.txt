project(asio_client)

add_executable(
	${PROJECT_NAME}
    asio_client.cpp
    can_net_comm.cpp
    frame.cpp
)

FILE(GLOB_RECURSE LibFiles "*.h")
add_custom_target(
	${PROJECT_NAME}_headers 
	SOURCES ${LibFiles}
)


target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
if(UNIX)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

#export CXX=/usr/bin/clang++ 
#export CC=/usr/bin/clang
