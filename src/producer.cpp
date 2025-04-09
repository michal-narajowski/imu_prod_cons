#include <thread>
#include <chrono>
#include <boost/log/trivial.hpp>

#include "producer.hpp"

Producer::Producer(std::unique_ptr<SocketSender> socket_handler,
                  std::unique_ptr<IImuDataProvider> data_provider,
                  int frequency_hz)
    : socket_handler_(std::move(socket_handler))
    , data_provider_(std::move(data_provider))
    , interval_ms_(1000 / frequency_hz) {}

void Producer::run() {
    while (true) {
        auto packet = data_provider_->get_next();
        
        if (!socket_handler_->send_data(packet)) {
            break;
        }
        
        BOOST_LOG_TRIVIAL(debug) << "Sent packet: " << packet.to_string();
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms_));
    }
}