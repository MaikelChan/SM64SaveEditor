
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

target_link_libraries(${PROJECT_NAME} glfw3 OpenGL::GL)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
