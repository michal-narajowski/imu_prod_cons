#include <cstring>
#include <unistd.h>
#include <poll.h>
#include <boost/log/trivial.hpp>

#include "socket_handler.hpp"

SocketHandler::SocketHandler(const std::string& socket_path)
    : sockfd_(-1)
    , socket_path_(socket_path) {}

SocketHandler::~SocketHandler() {
    if (sockfd_ != -1) {
        close(sockfd_);
    }
}

bool SocketHandler::initialize() {
    sockfd_ = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd_ == -1) {
        BOOST_LOG_TRIVIAL(error) << "Failed to create socket: " << strerror(errno);
        return false;
    }

    addr_.sun_family = AF_UNIX;
    std::strncpy(addr_.sun_path, socket_path_.c_str(), sizeof(addr_.sun_path) - 1);
    
    return true;
}

bool SocketSender::send_data(const ImuData_t& data) {
    if (sendto(sockfd_, &data, sizeof(data), 0, 
               reinterpret_cast<struct sockaddr*>(&addr_), 
               sizeof(addr_)) == -1) {
        BOOST_LOG_TRIVIAL(error) << "Failed to send data: " << strerror(errno);
        return false;
    }
    return true;
}

bool SocketReceiver::initialize() {
    if (!SocketHandler::initialize()) {
        return false;
    }

    unlink(socket_path_.c_str());

    if (bind(sockfd_, reinterpret_cast<struct sockaddr*>(&addr_), 
             sizeof(addr_)) == -1) {
        BOOST_LOG_TRIVIAL(error) << "Failed to bind: " << strerror(errno);
        return false;
    }

    addr_len_ = sizeof(addr_);
    return true;
}

bool SocketReceiver::receive_data(ImuData_t& data, int timeout_ms) {
    pollfd pfd = {sockfd_, POLLIN, 0};
    int ret = poll(&pfd, 1, timeout_ms);

    if (ret == -1) {
        BOOST_LOG_TRIVIAL(error) << "Failed to poll: " << strerror(errno);
        return false;
    }

    if (ret == 0) {
        return false;  // Timeout
    }

    if (pfd.revents & POLLIN) {
        ssize_t bytes_received = recvfrom(sockfd_, &data, sizeof(data), 0,
                                        reinterpret_cast<struct sockaddr*>(&addr_),
                                        &addr_len_);
        
        if (bytes_received == -1) {
            BOOST_LOG_TRIVIAL(error) << "Failed to receive data: " << strerror(errno);
            return false;
        }
        return true;
    }

    return false;
}