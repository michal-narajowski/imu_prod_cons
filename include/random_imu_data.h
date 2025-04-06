#ifndef RANDOM_IMU_DATA_H
#define RANDOM_IMU_DATA_H

#include <random>

#include "imu_data.h"
#include "imu_data_provider.h"

class RandomImuData : public IImuDataProvider {
public:
    RandomImuData(unsigned int seed) :
        gen(seed),
        dist_float(-100.0f, 100.0f),
        dist_int(-100, 100),
        dist_uint(0, 1000)
    {}

    bool initialize() override {
        return true;
    }

    ImuData_t get_next() override {
        ImuData_t data;
        data.xAcc = dist_float(gen);
        data.yAcc = dist_float(gen);
        data.zAcc = dist_float(gen);
        data.timestampAcc = dist_uint(gen);
        data.xGyro = dist_int(gen);
        data.yGyro = dist_int(gen);
        data.zGyro = dist_int(gen);
        data.timestampGyro = dist_uint(gen);
        data.xMag = dist_float(gen);
        data.yMag = dist_float(gen);
        data.zMag = dist_float(gen);
        data.timestampMag = dist_uint(gen);
        return data;
    }

private:
    std::mt19937 gen;
    std::uniform_real_distribution<float> dist_float;
    std::uniform_int_distribution<int32_t> dist_int;
    std::uniform_int_distribution<uint32_t> dist_uint;
};

#endif // RANDOM_IMU_DATA_H