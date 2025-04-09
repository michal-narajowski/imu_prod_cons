#pragma once
#include <memory>

#include "imu_data_provider.hpp"

class ImuDataFactory {
public:
    virtual std::unique_ptr<IImuDataProvider> create_random_imu_data(unsigned int seed) = 0;
    virtual std::unique_ptr<IImuDataProvider> create_csv_imu_data(const std::string& csv_file_path) = 0;
    virtual ~ImuDataFactory() = default;
};