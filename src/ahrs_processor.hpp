#pragma once
#include "Fusion.h"
#include "imu_data.hpp"

class AhrsProcessor {
public:
    AhrsProcessor();
    void initialize();
    void process_imu_data(const ImuData_t& data);
    FusionEuler get_orientation() const;

private:
    FusionAhrs ahrs_;
    uint32_t last_ts_;
    float calculate_delta_time(uint32_t current_ts) const;
};