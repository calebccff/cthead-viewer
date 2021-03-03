cmake_minimum_required (VERSION 2.8)
project (CTHeadViewer)

#version number
set (CTHeadViewer_VERSION_MAJOR 1)
set (CTHeadViewer_VERSION_MINOR 0)

find_package(ImGui-SFML REQUIRED)
find_package (Eigen3 3.3 CONFIG REQUIRED) # NO MODULE

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

#include the subdirectory containing our libs
include_directories(Viewer)
include_directories(Window)

set(Viewer_Files Viewer/File.cc
    Viewer/View.cc
    Viewer/View_renderFuncs.cc)

set(Window_Files Window/Window.cc)

#indicate the entry point for the executable
add_executable (CTHeadViewer
    CTHeadViewer.cc
    ${Viewer_Files}
    ${Window_Files})

# Indicate which libraries to include during the link process.
target_link_libraries (CTHeadViewer ImGui-SFML::ImGui-SFML Eigen3::Eigen)

install (TARGETS CTHeadViewer DESTINATION bin)