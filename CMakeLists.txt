# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.11)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("SM64SaveEditor" VERSION "1.2.0")
configure_file("${CMAKE_CURRENT_LIST_DIR}/src/Config.h.in" "${CMAKE_CURRENT_LIST_DIR}/src/Config.h")
configure_file("${CMAKE_CURRENT_LIST_DIR}/src/resources.rc.in" "${CMAKE_CURRENT_LIST_DIR}/src/resources.rc")

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_LIST_DIR}/src/resources.rc")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(glfw3 3.3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

# Set OpenGL preference. On Linux there may be two gl libraries.
# Specify with "LEGACY" or "GLVND". GLVND requires Cmake 3.11.
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Include sub-projects.
add_subdirectory ("src")