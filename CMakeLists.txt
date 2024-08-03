cmake_minimum_required(VERSION 3.10)

project(Ensemble_Classifier)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(SRC_DIR src)
set(INCLUDE_DIR include)

file(GLOB ENSEMBLE_SOURCES "${SRC_DIR}/ensemble/*.cpp")
file(GLOB LINEAR_SOURCES "${SRC_DIR}/linear/*.cpp")
file(GLOB VOTER_SOURCES "${SRC_DIR}/voter/*.cpp")
file(GLOB COMMON "${SRC_DIR}/utils/utils.cpp")

include_directories(${INCLUDE_DIR})

add_executable(ensemble ${ENSEMBLE_SOURCES} ${COMMON})

add_executable(linear ${LINEAR_SOURCES} ${COMMON})

add_executable(voter ${VOTER_SOURCES} ${COMMON})