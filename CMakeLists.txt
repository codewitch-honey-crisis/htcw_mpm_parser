cmake_minimum_required(VERSION 3.24)
project(htcw_mpm_parser VERSION 1.0)
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_library(htcw_mpm_parser src/source/mpm_parser.c)
target_include_directories(htcw_mpm_parser PUBLIC
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_BINARY_DIR}"
)
