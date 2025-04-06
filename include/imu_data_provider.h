#ifndef IMU_DATA_PROVIDER_H
#define IMU_DATA_PROVIDER_H

#include "imu_data.h"

class IImuDataProvider {
public:
    virtual ~IImuDataProvider() = default;
    virtual bool initialize() = 0;
    virtual ImuData_t get_next() = 0;
};

#endif // IMU_DATA_PROVIDER_H