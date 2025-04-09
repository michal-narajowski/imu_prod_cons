#pragma once
#include <memory>

#include "socket_handler.hpp"
#include "imu_data_factory.hpp"

class Producer {
public:
    Producer(std::unique_ptr<SocketSender> socket_handler,
             std::unique_ptr<IImuDataProvider> data_provider,
             int frequency_hz);
    
    void run();
    
private:
    std::unique_ptr<SocketSender> socket_handler_;
    std::unique_ptr<IImuDataProvider> data_provider_;
    int interval_ms_;
};