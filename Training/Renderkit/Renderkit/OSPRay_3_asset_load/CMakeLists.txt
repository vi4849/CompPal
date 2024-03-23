cmake_minimum_required(VERSION 3.16)
project(asset_loader LANGUAGES C CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ospray REQUIRED)

set(ONEAPI_ROOT "")
if($ENV{ONEAPI_ROOT})
	set(ONEAPI_ROOT $ENV{ONEAPI_ROOT})
else()
	if(WIN32)
		set(ONEAPI_ROOT "C:/Program Files (x86)/Intel/oneAPI")
	else()
		set(ONEAPI_ROOT /opt/intel/oneapi)
	endif()
endif($ENV{ONEAPI_ROOT})


message(STATUS "ONEAPI_ROOT: ${ONEAPI_ROOT}")
set(TRAINING_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(GLM_ROOT ${TRAINING_ROOT}/third-party/glm)
set(STB_ROOT ${TRAINING_ROOT}/third-party)
set(TINYOBJ_ROOT ${TRAINING_ROOT}/third-party)

include_directories(${OSPRAY_INCLUDE_DIR} ${RKCOMMON_INCLUDE_DIRS} ${GLM_ROOT}/include ${STB_ROOT} ${TINYOBJ_ROOT})
link_directories(${OSPRAY_ROOT}/lib ${ONEAPI_ROOT}/rkcommon/latest/lib)

add_executable(${PROJECT_NAME} ${OSPRAY_RESOURCE} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ospray rkcommon stdc++fs)

install(TARGETS ${PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)
