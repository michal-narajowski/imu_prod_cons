#pragma once
#include <string>
#include <sys/socket.h>
#include <sys/un.h>

#include "imu_data.hpp"

class SocketHandler {
public:
    explicit SocketHandler(const std::string& socket_path);
    virtual ~SocketHandler();
    
    virtual bool initialize();
    
protected:
    int sockfd_;
    std::string socket_path_;
    sockaddr_un addr_;
};

class SocketSender : public SocketHandler {
public:
    using SocketHandler::SocketHandler;
    bool send_data(const ImuData_t& data);
};

class SocketReceiver : public SocketHandler {
public:
    using SocketHandler::SocketHandler;
    bool initialize() override;
    bool receive_data(ImuData_t& data, int timeout_ms);

private:
    socklen_t addr_len_;
};