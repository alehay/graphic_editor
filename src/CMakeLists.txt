# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.10)

project(graphics_library VERSION 0.0.1 LANGUAGES C)

add_library(graphics SHARED
  "graphics.cpp"
)

set_target_properties(graphics PROPERTIES
  PUBLIC_HEADER graphics.hpp
  OUTPUT_NAME "graphics"
)

target_compile_definitions(graphics PUBLIC DART_SHARED_LIB)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( graphics ${OpenCV_LIBS} )