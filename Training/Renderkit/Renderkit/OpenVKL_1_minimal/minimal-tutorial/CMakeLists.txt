# Build using
# cmake ..
# With:
# export LD_LIBRARY_PATH=/path/to/openvkl-2.0.0-sycl/lib
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(openvkl_webinar_2.0.0 LANGUAGES CXX)

find_package(openvkl 2.0.0 CONFIG REQUIRED)

add_executable(minimal src/minimal_01_sol.cpp)
##add_executable(minimal src/minimal_02_sol.cpp)
##add_executable(minimal src/minimal_03_sol.cpp)
##add_executable(minimal src/minimal_04_sol.cpp)
##add_executable(minimal src/minimal_05_sol.cpp)
##add_executable(minimal src/minimal_06_sol.cpp)

target_link_libraries(minimal PRIVATE openvkl::openvkl openvkl::openvkl_module_cpu_device)


## FOR GPU USAGE OF OpenVKL

## STEP 1: MODIFY THE TARGET
    ## target_link_libraries(minimal_06_sol PRIVATE openvkl::openvkl openvkl::openvkl_module_gpu_device)

## STEP 2: MODIFY THE COMPILER
    ## https://github.com/intel/llvm/releases/download/nightly-2023-09-22/sycl_linux.tar.gz

## STEP 3: LINK TO THE COMPILER
    ## cmake -DCMAKE_CXX_COMPILER=/path/to/clang/clang++ ..