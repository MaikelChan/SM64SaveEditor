
file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_CURRENT_LIST_DIR}/*.c
	${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

#file(GLOB_RECURSE HEADER_FILES 
#	${CMAKE_CURRENT_LIST_DIR}/*.h
#	${CMAKE_CURRENT_LIST_DIR}/*.hpp
#)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${SOURCE_FILES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(${PROJECT_NAME} glfw3 OpenGL::GL)
else()
	target_link_libraries(${PROJECT_NAME} glfw OpenGL::GL)
endif()

# imfilebrowser requires c++17.
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# TODO: Add tests and install targets if needed.
