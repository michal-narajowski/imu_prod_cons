#pragma once
#include "imu_data.hpp"

class IImuDataProvider {
public:
    virtual ~IImuDataProvider() = default;
    virtual bool initialize() = 0;
    virtual ImuData_t get_next() = 0;
};