#pragma once
#include <memory>

#include "socket_handler.hpp"
#include "ahrs_processor.hpp"

class Consumer {
public:
    Consumer(std::unique_ptr<SocketReceiver> socket_receiver,
            std::unique_ptr<AhrsProcessor> ahrs_processor,
            int timeout_ms);
    
    void run();
    
private:
    std::unique_ptr<SocketReceiver> socket_receiver_;
    std::unique_ptr<AhrsProcessor> ahrs_processor_;
    int timeout_ms_;
};