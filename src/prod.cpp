#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctime>
#include <chrono>
#include <thread>
#include <random>

struct __attribute__((packed)) DataPacket {
    float x;
    float y;
    float z;
};

void print_usage(const char* program_name) {
    std::cerr << "Usage: " << program_name << " <socket_path> <interval_ms>\n";
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* socket_path = argv[1];
    int interval_ms = std::stoi(argv[2]);

    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    unlink(socket_path);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(sockfd);
        return EXIT_FAILURE;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    while (true) {
        DataPacket packet{dist(gen), dist(gen), dist(gen)};

        if (sendto(sockfd, &packet, sizeof(packet), 0, (sockaddr*)&addr, sizeof(addr)) == -1) {
            perror("sendto");
            close(sockfd);
            return EXIT_FAILURE;
        }

        std::cout << "Sent packet: x=" << packet.x << ", y=" << packet.y << ", z=" << packet.z << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }

    close(sockfd);
    return EXIT_SUCCESS;
}