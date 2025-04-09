cmake_minimum_required(VERSION 3.10)

project(imu_prod_cons)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost CONFIG 1.83.0 COMPONENTS program_options log log_setup REQUIRED)
add_subdirectory("Fusion/Fusion")

include_directories(${Boost_INCLUDE_DIRS}) 
include_directories("Fusion/Fusion")

add_executable(producer src/prod_main.cpp)
add_executable(consumer src/cons_main.cpp)

target_sources(producer 
    PRIVATE 
    src/producer.cpp 
    src/log_utils.cpp 
    src/socket_handler.cpp 
    src/csv_imu_data.cpp
    src/concrete_imu_data_factory.cpp
)

target_sources(consumer 
    PRIVATE 
    src/consumer.cpp 
    src/log_utils.cpp 
    src/socket_handler.cpp 
    src/ahrs_processor.cpp 
)

target_link_libraries(producer ${Boost_LIBRARIES})
target_link_libraries(consumer ${Boost_LIBRARIES} Fusion)