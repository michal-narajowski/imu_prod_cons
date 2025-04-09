#include <boost/log/trivial.hpp>
#include "ahrs_processor.hpp"

AhrsProcessor::AhrsProcessor() : last_ts_(0) {}

void AhrsProcessor::initialize() {
    FusionAhrsInitialise(&ahrs_);
}

void AhrsProcessor::process_imu_data(const ImuData_t& data) {
    const FusionVector gyroscope = {
        static_cast<float>(data.xGyro * (M_PI / 180.0f)),
        static_cast<float>(data.yGyro * (M_PI / 180.0f)),
        static_cast<float>(data.zGyro * (M_PI / 180.0f))
    };
    
    const FusionVector accelerometer = {
        static_cast<float>(data.xAcc / 1000.0f),
        static_cast<float>(data.yAcc / 1000.0f),
        static_cast<float>(data.zAcc / 1000.0f)
    };
    
    const FusionVector magnetometer = {
        static_cast<float>(data.xMag / 1000.0f),
        static_cast<float>(data.yMag / 1000.0f),
        static_cast<float>(data.zMag / 1000.0f)
    };

    auto mean_timestamp = (data.timestampMag + data.timestampGyro + data.timestampAcc) / 3;
    float delta_time = calculate_delta_time(mean_timestamp);
    last_ts_ = mean_timestamp;

    FusionAhrsUpdate(&ahrs_, gyroscope, accelerometer, magnetometer, delta_time);
}

float AhrsProcessor::calculate_delta_time(uint32_t current_ts) const {
    if (last_ts_ == 0) {
        return 0.0f;
    }
    
    auto delta_ts = current_ts - last_ts_;
    return static_cast<float>(delta_ts) / 10e9f; // ns to s
}

FusionEuler AhrsProcessor::get_orientation() const {
    return FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs_));
}