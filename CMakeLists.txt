cmake_minimum_required(VERSION 3.10)

project(imu_prod_cons)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.83.0 COMPONENTS program_options log log_setup REQUIRED)
add_subdirectory("Fusion/Fusion")

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS}) 
include_directories("Fusion/Fusion")

add_executable(producer src/prod.cpp src/log_utils.cpp)
add_executable(consumer src/cons.cpp src/log_utils.cpp)

target_link_libraries(producer ${Boost_LIBRARIES})
target_link_libraries(consumer ${Boost_LIBRARIES} Fusion)