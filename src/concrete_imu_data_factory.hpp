#pragma once
#include <memory>
#include "imu_data_factory.hpp"
#include "imu_data_provider.hpp"

class ConcreteImuDataFactory : public ImuDataFactory {
public:
    std::unique_ptr<IImuDataProvider> create_random_imu_data(unsigned int seed) override;
    std::unique_ptr<IImuDataProvider> create_csv_imu_data(const std::string& csv_file_path) override;
};