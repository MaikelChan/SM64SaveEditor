
file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/include/imgui/*.cpp
	${CMAKE_CURRENT_LIST_DIR}/*.c
	${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

#file(GLOB_RECURSE HEADER_FILES 
#	${CMAKE_CURRENT_LIST_DIR}/*.h
#	${CMAKE_CURRENT_LIST_DIR}/*.hpp
#)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${SOURCE_FILES} ${APP_ICON_RESOURCE_WINDOWS})

if(MSVC)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SUBSYSTEM "CONSOLE")
  else()
    set(SUBSYSTEM "WINDOWS")
  endif()
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:${SUBSYSTEM}")
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /wd4458 /wd4100)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3-static winmm setupapi version imm32)

# C++ 17 to be able to use std::filesystem
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# TODO: Add tests and install targets if needed.
