#include <boost/log/trivial.hpp>
#include <iomanip>

#include "consumer.hpp"

Consumer::Consumer(std::unique_ptr<SocketReceiver> socket_receiver,
                  std::unique_ptr<AhrsProcessor> ahrs_processor,
                  int timeout_ms)
    : socket_receiver_(std::move(socket_receiver))
    , ahrs_processor_(std::move(ahrs_processor))
    , timeout_ms_(timeout_ms) {}

void Consumer::run() {
    ImuData_t packet;
    
    while (true) {
        if (!socket_receiver_->receive_data(packet, timeout_ms_)) {
            BOOST_LOG_TRIVIAL(warning) << "Timeout waiting for data";
            continue;
        }

        BOOST_LOG_TRIVIAL(debug) << "Received packet: " << packet.to_string();
        
        try {
            ahrs_processor_->process_imu_data(packet);
            
            const auto euler = ahrs_processor_->get_orientation();
            BOOST_LOG_TRIVIAL(info) << std::fixed << std::setprecision(1) 
                << "Roll: " << euler.angle.roll 
                << ", Pitch: " << euler.angle.pitch 
                << ", Yaw: " << euler.angle.yaw;
        } catch (const std::exception& ex) {
            BOOST_LOG_TRIVIAL(error) << "Error processing IMU data: " << ex.what();
        }
    }
}