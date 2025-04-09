#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "imu_data.hpp"
#include "imu_data_provider.hpp"

class CsvImuData : public IImuDataProvider {
public:
    explicit CsvImuData(const std::string& csv_file_path);
    bool initialize() override;
    ImuData_t get_next() override;

private:
    std::chrono::nanoseconds float_seconds_to_nanoseconds(float seconds);
    void load_data();

    std::string file_path;
    std::vector<ImuData_t> imu_data_vector;
    size_t data_index = 0;
    std::chrono::steady_clock::time_point start_time;
};