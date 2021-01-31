cmake_minimum_required (VERSION 2.8)
project (CTHeadViewer)

#version number
set (CTHeadViewer_VERSION_MAJOR 1)
set (CTHeadViewer_VERSION_MINOR 0)

#include the subdirectory containing our libs
add_subdirectory (Viewer)
include_directories(Viewer)
#indicate the entry point for the executable
add_executable (CTHeadViewer Viewer CTHeadViewer.cc)

# Indicate which libraries to include during the link process.
target_link_libraries (CTHeadViewer PRIVATE Viewer ImGui-SFML::ImGui-SFML)

find_package(ImGui-SFML REQUIRED)

install (TARGETS CTHeadViewer DESTINATION bin)