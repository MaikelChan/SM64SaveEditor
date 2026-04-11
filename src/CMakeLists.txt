
configure_file("${CMAKE_CURRENT_LIST_DIR}/Config.h.in" "${CMAKE_CURRENT_LIST_DIR}/Config.h")
configure_file("${CMAKE_CURRENT_LIST_DIR}/resources.rc.in" "${CMAKE_CURRENT_LIST_DIR}/resources.rc")

file(GLOB_RECURSE SOURCE_FILES 
    ${CMAKE_SOURCE_DIR}/include/imgui/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.c
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

add_executable (${PROJECT_NAME} ${SOURCE_FILES} "${CMAKE_CURRENT_LIST_DIR}/resources.rc")

# C++ 17 to be able to use std::filesystem
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${FETCHCONTENT_BASE_DIR}/sdl3-src/include"
    "${CMAKE_SOURCE_DIR}/include"
)

if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(SUBSYSTEM "CONSOLE")
    else()
        set(SUBSYSTEM "WINDOWS")
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:${SUBSYSTEM}")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /wd4100)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

if (LINK_SDL3_STATICALLY)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-static)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared)
endif()

# Transparent windows seems to be problematic on some systems, so disable for now
#target_compile_definitions(${PROJECT_NAME} PRIVATE SUPPORT_TRANSPARENCY)

# Post build: copy the SDL3.dll to the executable's path

if (WIN32 AND (NOT LINK_SDL3_STATICALLY))
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM
    )
endif()
