#ifndef CONCRETE_IMU_DATA_FACTORY_H
#define CONCRETE_IMU_DATA_FACTORY_H

#include <memory>

#include "imu_data_factory.h"
#include "imu_data_provider.h"
#include "random_imu_data.h"
#include "csv_imu_data.h"

class ConcreteImuDataFactory : public ImuDataFactory {
public:
    std::unique_ptr<IImuDataProvider> create_random_imu_data(unsigned int seed) override {
        return std::make_unique<RandomImuData>(seed);
    }
    std::unique_ptr<IImuDataProvider> create_csv_imu_data(const std::string& csv_file_path) override {
        return std::make_unique<CsvImuData>(csv_file_path);
    }
};

#endif // CONCRETE_IMU_DATA_FACTORY_H