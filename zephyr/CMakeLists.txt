# Copyright (c) 2025 honey the codewitch
#
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_HTCW_MPM_PARSER)
zephyr_library()
zephyr_include_directories(../src)
zephyr_library_include_directories(../src)
zephyr_interface_library_named(HTCW_MPM_PARSER)
zephyr_library_sources(../src/source/mpm_parser.c)
#zephyr_library_sources_ifdef(CONFIG_PARAM_DEF, file.cpp)
zephyr_library_link_libraries(HTCW_MPM_PARSER)
target_link_libraries(HTCW_MPM_PARSER INTERFACE zephyr_interface)

endif()