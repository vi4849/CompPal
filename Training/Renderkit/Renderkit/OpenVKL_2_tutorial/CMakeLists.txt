## Copyright 2019-2020 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(vklTutorial LANGUAGES C)

foreach(p
CMP0048 # OK to clear PROJECT_VERSION on project()
CMP0054 # CMake 3.1
CMP0056 # export EXE_LINKER_FLAGS to try_run
CMP0057 # Support no if() IN_LIST operator
CMP0063 # Honor visibility properties for all targets
CMP0074 # Searches prefixes specified by the <PackageName>_ROOT
)
if(POLICY ${p})
cmake_policy(SET ${p} NEW)
endif()
endforeach()

find_package(openvkl REQUIRED)

add_executable(vklTutorial ${CMAKE_CURRENT_SOURCE_DIR}/vklTutorial.c)
target_link_libraries(vklTutorial openvkl::openvkl)