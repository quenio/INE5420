cmake_minimum_required(VERSION 3.5)
project(graphics)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# GTK3
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GTKMM3 REQUIRED gtkmm-3.0)
include_directories(${GTK3_INCLUDE_DIRS} ${GTKMM3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${GTKMM3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER} ${GTKMM3_CFLAGS_OTHER})

# CMake Target
set(SOURCE_FILES main.cpp ui.h graphics.h)
add_executable(graphics_cmake ${SOURCE_FILES})
target_link_libraries(graphics_cmake ${GTK3_LIBRARIES})

# Make Target
add_custom_target(graphics COMMAND make -C ${graphics_SOURCE_DIR}
                           CLION_EXE_DIR=${PROJECT_BINARY_DIR} SOURCE_FILES=${SOURCE_FILES})