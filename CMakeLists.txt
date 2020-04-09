cmake_minimum_required(VERSION 3.13)
project(TBB-study LANGUAGES CXX)

set(TBB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/oneTBB" CACHE PATH "")

include(${TBB_ROOT}/cmake/TBBBuild.cmake)
tbb_build(TBB_ROOT ${TBB_ROOT} CONFIG_DIR TBB_DIR)
find_package(TBB REQUIRED tbb)

find_package(Boost REQUIRED)

foreach(src IN ITEMS
  parallel_for.cpp
  parallel_pipeline.cpp
  parallel_reduce.cpp
  parallel_reducible_histogram.cpp
  parallel_scan.cpp
  task_group.cpp
)
  string(REPLACE ".cpp" "" x ${src})
  add_executable(${x} ${src})
  target_include_directories(${x} PRIVATE ${Boost_INCLUDE_DIRS})
  target_compile_features(${x} PRIVATE cxx_std_17)
  set_property(TARGET ${x} PROPERTY CXX_EXTENSIONS OFF)
  target_compile_options(${x} PRIVATE -Wall -Wextra)
  target_link_libraries(${x} ${TBB_IMPORTED_TARGETS})
endforeach()
