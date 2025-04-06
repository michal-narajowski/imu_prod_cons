cmake_minimum_required(VERSION 3.10)

project(imu_prod_cons)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.83.0 COMPONENTS program_options log log_setup REQUIRED)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS}) 

add_executable(producer src/prod.cpp)
add_executable(consumer src/cons.cpp)

target_link_libraries(producer ${Boost_LIBRARIES})
target_link_libraries(consumer ${Boost_LIBRARIES})