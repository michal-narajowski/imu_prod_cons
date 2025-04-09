#include "concrete_imu_data_factory.hpp"
#include "random_imu_data.hpp"
#include "csv_imu_data.hpp"

std::unique_ptr<IImuDataProvider> ConcreteImuDataFactory::create_random_imu_data(unsigned int seed) {
    return std::make_unique<RandomImuData>(seed);
}

std::unique_ptr<IImuDataProvider> ConcreteImuDataFactory::create_csv_imu_data(const std::string& csv_file_path) {
    return std::make_unique<CsvImuData>(csv_file_path);
}