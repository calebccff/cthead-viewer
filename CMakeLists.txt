cmake_minimum_required (VERSION 2.8)
project (CTHeadViewer)

#version number
set (CTHeadViewer_VERSION_MAJOR 1)
set (CTHeadViewer_VERSION_MINOR 0)

set (CMAKE_CXX_STANDARD 17)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

#include the subdirectory containing our libs
add_subdirectory (Viewer)
include_directories(Viewer)
#indicate the entry point for the executable
add_executable (CTHeadViewer Viewer CTHeadViewer.cc)

# Indicate which libraries to include during the link process.
target_link_libraries (CTHeadViewer PRIVATE Viewer ImGui-SFML::ImGui-SFML)

find_package(ImGui-SFML REQUIRED)

install (TARGETS CTHeadViewer DESTINATION bin)