cmake_minimum_required(VERSION 3.5)
project(graphics)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 \
    -Werror -Weverything \
    -Wno-header-hygiene -Wno-non-virtual-dtor -Wno-exit-time-destructors -Wno-global-constructors \
    -Wno-padded -Wno-weak-vtables -Wno-gnu-zero-variadic-macro-arguments \
    -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-old-style-cast")

# GTK3
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GTKMM3 REQUIRED gtkmm-3.0)
include_directories(${GTK3_INCLUDE_DIRS} ${GTKMM3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${GTKMM3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER} ${GTKMM3_CFLAGS_OTHER})

# Main Target
set(SOURCE_FILES ui.h graphics.h clipping.h clipping_cs.h clipping_lb.h region.h coord.h)
add_executable(graphics main.cpp ${SOURCE_FILES})
target_link_libraries(graphics ${GTK3_LIBRARIES})

# Unit Tests
set(MIN_UNIT_FILES ./tests/min_unit.cpp ./tests/min_unit.h ./tests/timer.cpp ./tests/timer.h)
add_executable(coord_tests ./tests/coord_tests.cpp ${MIN_UNIT_FILES} ${SOURCE_FILES})
add_executable(region_tests tests/region_tests.cpp ${MIN_UNIT_FILES} ${SOURCE_FILES})
add_executable(graphics_tests ./tests/graphics_tests.cpp ${MIN_UNIT_FILES} ${SOURCE_FILES})
