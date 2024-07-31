cmake_minimum_required(VERSION 3.10)

project(Ensemble_Classifier)

set(CMAKE_CXX_STANDARD 11)
set(SRC_DIR src)
set(INCLUDE_DIR include)

include_directories(${INCLUDE_DIR})

add_executable(ensemble_classifier ${SRC_DIR}/ensemble_classifier.cpp ${SRC_DIR}/utils.cpp)
add_executable(linear_classifier ${SRC_DIR}/linear_classifier.cpp ${SRC_DIR}/utils.cpp)
add_executable(voter ${SRC_DIR}/voter.cpp ${SRC_DIR}/utils.cpp)