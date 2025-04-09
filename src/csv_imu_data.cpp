#include "csv_imu_data.hpp"
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/log/trivial.hpp>
#include <cmath>

CsvImuData::CsvImuData(const std::string& csv_file_path) 
    : file_path(csv_file_path)
    , start_time(std::chrono::steady_clock::now()) {
}

bool CsvImuData::initialize() {
    try {
        load_data();
        return true;
    } catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << "Error initializing CsvImuData: " << e.what();
        return false;
    }
}

ImuData_t CsvImuData::get_next() {
    if (data_index >= imu_data_vector.size()) {
        data_index = 0;
    }
    return imu_data_vector[data_index++];
}

std::chrono::nanoseconds CsvImuData::float_seconds_to_nanoseconds(float seconds) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::duration<float>(seconds));
}

void CsvImuData::load_data() {
    std::ifstream file(file_path);
    std::string line;

    std::getline(file, line); // Skip header

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
            // Parse accelerometer data
            imu_data.xAcc = std::stof(tokens[1]) * ms2_to_mg;
            imu_data.yAcc = std::stof(tokens[2]) * ms2_to_mg;
            imu_data.zAcc = std::stof(tokens[3]) * ms2_to_mg;
            imu_data.timestampAcc = (start_time + float_seconds_to_nanoseconds(std::stof(tokens[0]))).time_since_epoch().count();

            // Parse gyroscope data
            imu_data.xGyro = static_cast<int32_t>(round(std::stof(tokens[5]) * rads_to_mdegs));
            imu_data.yGyro = static_cast<int32_t>(round(std::stof(tokens[6]) * rads_to_mdegs));
            imu_data.zGyro = static_cast<int32_t>(round(std::stof(tokens[7]) * rads_to_mdegs));
            imu_data.timestampGyro = (start_time + float_seconds_to_nanoseconds(std::stof(tokens[4]))).time_since_epoch().count();

            // Parse magnetometer data
            imu_data.xMag = std::stof(tokens[9]) * utesla_to_mgauss;
            imu_data.yMag = std::stof(tokens[10]) * utesla_to_mgauss;
            imu_data.zMag = std::stof(tokens[11]) * utesla_to_mgauss;
            imu_data.timestampMag = (start_time + float_seconds_to_nanoseconds(std::stof(tokens[8]))).time_since_epoch().count();

        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error parsing data") + line_number_info + ": " + e.what());
        }

        imu_data_vector.push_back(imu_data);
    }

    if (imu_data_vector.empty()) {
        throw std::runtime_error("No data loaded from CSV file.");
    }
}