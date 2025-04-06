cmake_minimum_required(VERSION 3.10)

project(imu_prod_cons)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(producer src/prod.cpp)
add_executable(consumer src/cons.cpp)
