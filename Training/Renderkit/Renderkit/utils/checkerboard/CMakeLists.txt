## Copyright 2019 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

# NOTE: This CMakeLists.txt is intended to be used to exercise an OSPRay
#       install and demonstrate how external applications can build against
#       OSPRay using CMake.
#
#       Once you have done a 'make install' of an existing OSPRay
#       build (will install to CMAKE_INSTALL_PREFIX), create a separate build
#       directory and invoke CMake on this directory. If you have 'ospray_DIR'
#       setup correctly to point to where you just installed OSPRay, then this
#       should build the ospTutorial app from that install and NOT use your
#       local build.

cmake_minimum_required(VERSION 3.1)

project(checkerboard LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

set(ONEAPI_ROOT /opt/intel/inteloneapi/)
set(OSPRAY_ROOT ${ONEAPI_ROOT}/ospray/latest)
set(OSPCOMMON_ROOT ${ONEAPI_ROOT}/ospcommon/latest)
set(EMBREE_ROOT ${ONEAPI_ROOT}/embree/latest)
set(OPENVKL_ROOT ${ONEAPI_ROOT}/openvkl/latest)
set(OIDN_ROOT ${ONEAPI_ROOT}/oidn/latest)

list(APPEND CMAKE_PREFIX_PATH 
${OSPCOMMON_ROOT}/lib/cmake/ospcommon-1.3.0/
${OSPRAY_ROOT}/lib/cmake/ospray-2.1.0/
${EMBREE_ROOT}/lib/cmake/embree-3.9.0/
${OPENVKL_ROOT}/lib/cmake/openvkl-0.9.0/
${OIDN_ROOT}/lib/cmake/OpenImageDenoise/
/home/u40231/renderkit-notebooks/notebooks/lib/
)

find_package(ospray 2.0.0 REQUIRED)
find_package(gflags REQUIRED)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/checkerboard.cpp)
target_include_directories(${PROJECT_NAME} 
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} 
PUBLIC ${OSPRAY_ROOT}/include/ospray
PUBLIC ${OSPRAY_ROOT}/include/ospray/ospray_testing
PUBLIC ${OSPRAY_ROOT}/src/ospray/apps
PUBLIC ${OSPRAY_ROOT}/src/ospray/apps/common/arcball_camera
PUBLIC ${OSPRAY_ROOT}/include
PUBLIC ${OSPCOMMON_ROOT}/include
SYSTEM BEFORE PUBLIC /home/u40231/Projects/renderkit-notebooks/stb_image
)
target_link_directories(${PROJECT_NAME} 
PUBLIC ${OSPCOMMON_ROOT}/lib
PUBLIC ${OSPRAY_ROOT}/lib
PUBLIC /home/u40231/renderkit-notebooks/notebooks/lib/
)
target_link_libraries(${PROJECT_NAME} 
PUBLIC ospcommon ospray ospray_testing gflags Threads::Threads)

# set_target_properties(${PROJECT_NAME}
#     PROPERTIES
#         CXX_STANDARD 11
#         CXX_STANDARD_REQUIRED YES
#         CXX_EXTENSIONS YES
# )

