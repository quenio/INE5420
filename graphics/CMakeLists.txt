cmake_minimum_required(VERSION 3.5)
project(graphics)

set(SOURCE_FILES graphics.cpp)

add_custom_target(graphics COMMAND make -C ${graphics_SOURCE_DIR}
                           CLION_EXE_DIR=${PROJECT_BINARY_DIR} SOURCE_FILES=${SOURCE_FILES})