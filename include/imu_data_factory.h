#ifndef IMU_DATA_FACTORY_H
#define IMU_DATA_FACTORY_H

#include "imu_data_provider.h"

#include <memory>

class ImuDataFactory {
public:
    virtual std::unique_ptr<IImuDataProvider> create_random_imu_data(unsigned int seed) = 0;
    virtual std::unique_ptr<IImuDataProvider> create_csv_imu_data(const std::string& csv_file_path) = 0;
    virtual ~ImuDataFactory() = default;
};

#endif // IMU_DATA_FACTORY_H