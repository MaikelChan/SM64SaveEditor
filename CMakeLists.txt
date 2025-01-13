# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.

# Used features that require certain minimum version:
# * 3.11: Required for setting GLVND OpenGL preference.
# * 3.15: Removes the '/W3' MSVC warning flag added by CMake by default, so another warning level can be set with 'target_compile_options' without causing warnings.
cmake_minimum_required (VERSION 3.15)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("SM64SaveEditor" VERSION "1.2.3")
configure_file("${CMAKE_CURRENT_LIST_DIR}/src/Config.h.in" "${CMAKE_CURRENT_LIST_DIR}/src/Config.h")
configure_file("${CMAKE_CURRENT_LIST_DIR}/src/resources.rc.in" "${CMAKE_CURRENT_LIST_DIR}/src/resources.rc")

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_LIST_DIR}/src/resources.rc")

include_directories(${CMAKE_SOURCE_DIR}/include)

# Set OpenGL preference. On Linux there may be two gl libraries.
# Specify with "LEGACY" or "GLVND".
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

if (MSVC)
  # With MSVC on Windows, just put the static glfw3.lib in the lib/ folder
  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/glfw3.lib")
    message(SEND_ERROR "glfw3.lib is missing from '${CMAKE_SOURCE_DIR}/lib' folder.")
  endif()
  link_directories(${CMAKE_SOURCE_DIR}/lib)
else()
  # On Linux or on Windows with Mingw, having installed the glfw3 dev package or compiled and installed from source,
  # should have also installed "glfw3Config.cmake", which will be used by find_package.
  find_package(glfw3 3.3 REQUIRED)
  message(STATUS "Using 'glfw3Config.cmake' file located at '${glfw3_DIR}'.")
endif()

# Include sub-projects.
add_subdirectory ("src")