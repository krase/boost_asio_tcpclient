project(asio_server)


add_executable(
	${PROJECT_NAME}
    main.cpp
    tcpserver.cpp
    tcpserversession.cpp
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

