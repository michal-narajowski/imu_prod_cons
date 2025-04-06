#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <linux/in.h>
#include <boost/program_options.hpp>
#include <poll.h>

#include "payload.h"
#include "log_utils.h"

namespace po = boost::program_options;
namespace logging = boost::log;

int main(int argc, char* argv[]) {
    std::string socket_path;
    std::string log_level_string = "info";
    int timeout_ms = 1000;

    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("socket-path,s", po::value<std::string>(&socket_path)->required(), "socket path")
            ("log-level,l", po::value<std::string>(&log_level_string), "log level (trace, debug, info, warning, error, fatal)")
            ("timeout,t", po::value<int>(&timeout_ms), "timeout in milliseconds")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return EXIT_SUCCESS;
        }

        po::notify(vm);
    } catch (const po::error& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return EXIT_FAILURE;
    }

    logging::trivial::severity_level log_level;
    try {
        log_level = parse_log_level(log_level_string);
    } catch (const std::runtime_error& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return EXIT_FAILURE;
    }

    init_logging(log_level);

    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    unlink(socket_path.c_str());

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(sockfd);
        return EXIT_FAILURE;
    }

    Payload_IMU_t packet;
    socklen_t addr_len = sizeof(addr);

    while (true) {
        pollfd pfd = {sockfd, POLLIN, 0};
        int ret = poll(&pfd, 1, timeout_ms);

        if (ret == -1) {
            perror("poll");
            close(sockfd);
            return EXIT_FAILURE;
        } else if (ret == 0) {
            BOOST_LOG_TRIVIAL(warning) << "Timeout waiting for data";
            continue;
        }

        if (pfd.revents & POLLIN) {
            ssize_t bytes_received = recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&addr, &addr_len);
            if (bytes_received == -1) {
                perror("recvfrom");
                close(sockfd);
                return EXIT_FAILURE;
            }

            BOOST_LOG_TRIVIAL(debug) << "Received packet: xAcc=" << packet.xAcc << ", yAcc=" << packet.yAcc << ", zAcc=" << packet.zAcc
                      << ", tsAcc=" << packet.timestampAcc << ", xGyro=" << packet.xGyro << ", yGyro=" << packet.yGyro
                      << ", zGyro=" << packet.zGyro << ", tsGyro=" << packet.timestampGyro << ", xMag=" << packet.xMag
                      << ", yMag=" << packet.yMag << ", zMag=" << packet.zMag << ", tsMag=" << packet.timestampMag;
        }
    }

    close(sockfd);
    return EXIT_SUCCESS;
}