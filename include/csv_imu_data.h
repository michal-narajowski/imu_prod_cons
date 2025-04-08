#ifndef CSV_IMU_DATA_H
#define CSV_IMU_DATA_H

#include "imu_data.h"
#include "imu_data_provider.h"
#include <fstream>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <chrono>
#include <cmath>
#include <limits>

class CsvImuData : public IImuDataProvider {
public:
    CsvImuData(const std::string& csv_file_path) : file_path(csv_file_path), start_time(std::chrono::steady_clock::now()) {
    }

    bool initialize() override {
        try {
            load_data();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error initializing CsvImuData: " << e.what() << std::endl;
            return false;
        }
    }

    ImuData_t get_next() override {
        if (data_index >= imu_data_vector.size()) {
            data_index = 0;
        }
        return imu_data_vector[data_index++];
    }

private:
    std::chrono::nanoseconds float_seconds_to_nanoseconds(float seconds) {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<float>(seconds));
    }

    void load_data() {
        std::ifstream file(file_path);
        std::string line;

        std::getline(file, line);

        while (std::getline(file, line)) {
            ImuData_t imu_data;
            boost::tokenizer<boost::escaped_list_separator<char>> tok(line);
            std::vector<std::string> tokens;
            for (auto const& t : tok) {
                tokens.push_back(t);
            }

            if (tokens.size() != 12) {
                throw std::runtime_error("Invalid CSV format: Incorrect number of fields.");
            }
            const float g = 9.81f;
            const float ms2_to_mg = 1000.0f / g;
            const float rads_to_mdegs = 180000.0f / M_PI;
            const float utesla_to_mgauss = 10.0f;
            std::string line_number_info = " at line: " + std::to_string(imu_data_vector.size() + 2);

            try {
                imu_data.xAcc = std::stof(tokens[1]) * ms2_to_mg;
            } catch (...) {
                throw std::runtime_error("Invalid value for xAcc: " + tokens[1] + line_number_info);
            }

            try {
                imu_data.yAcc = std::stof(tokens[2]) * ms2_to_mg;
            } catch (...) {
                throw std::runtime_error("Invalid value for yAcc: " + tokens[2] + line_number_info);
            }

            try {
                imu_data.zAcc = std::stof(tokens[3]) * ms2_to_mg;
            } catch (...) {
                throw std::runtime_error("Invalid value for zAcc: " + tokens[3] + line_number_info);
            }

            try {
                imu_data.timestampAcc = (start_time + float_seconds_to_nanoseconds(std::stof(tokens[0]))).time_since_epoch().count();
            } catch (...) {
                throw std::runtime_error("Invalid value for timestampAcc: " + tokens[0] + line_number_info);
            }

            try {
                imu_data.xGyro = static_cast<int32_t>(round(std::stof(tokens[5]) * rads_to_mdegs));
            } catch (...) {
                throw std::runtime_error("Invalid value for xGyro: " + tokens[5] + line_number_info);
            }

            try {
                imu_data.yGyro = static_cast<int32_t>(round(std::stof(tokens[6]) * rads_to_mdegs));
            } catch (...) {
                throw std::runtime_error("Invalid value for yGyro: " + tokens[6] + line_number_info);
            }

            try {
                imu_data.zGyro = static_cast<int32_t>(round(std::stof(tokens[7]) * rads_to_mdegs));
            } catch (...) {
                throw std::runtime_error("Invalid value for zGyro: " + tokens[7] + line_number_info);
            }

            try {
                imu_data.timestampGyro = (start_time + float_seconds_to_nanoseconds(std::stof(tokens[4]))).time_since_epoch().count();
            } catch (...) {
                throw std::runtime_error("Invalid value for timestampGyro: " + tokens[4] + line_number_info);
            }

            try {
                imu_data.xMag = std::stof(tokens[9]) * utesla_to_mgauss;
            } catch (...) {
                throw std::runtime_error("Invalid value for xMag: " + tokens[9] + line_number_info);
            }

            try {
                imu_data.yMag = std::stof(tokens[10]) * utesla_to_mgauss;
            } catch (...) {
                throw std::runtime_error("Invalid value for yMag: " + tokens[10] + line_number_info);
            }

            try {
                imu_data.zMag = std::stof(tokens[11]) * utesla_to_mgauss;
            } catch (...) {
                throw std::runtime_error("Invalid value for zMag: " + tokens[11] + line_number_info);
            }

            try {
                imu_data.timestampMag = (start_time + float_seconds_to_nanoseconds(std::stof(tokens[8]))).time_since_epoch().count();
            } catch (...) {
                throw std::runtime_error("Invalid value for timestampMag: " + tokens[8] + line_number_info);
            }

            imu_data_vector.push_back(imu_data);
        }

        if (imu_data_vector.empty()) {
            throw std::runtime_error("No data loaded from CSV file.");
        }
    }

private:
    std::string file_path;
    std::vector<ImuData_t> imu_data_vector;
    size_t data_index = 0;
    std::chrono::steady_clock::time_point start_time;
};

#endif // CSV_IMU_DATA_H