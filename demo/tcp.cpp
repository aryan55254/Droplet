#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../include/TokenBucket.hpp"  
#include "../include/BucketManager.hpp"

BucketManager rateLimiter;

void handle_client(int client_socket, std::string ip)
{
    auto bucket = rateLimiter.GetBucket(ip, 10, 5);

    std::string response;
    if (bucket->allow(1))
        response = "OK\n";
    else
        response = "LIMIT\n";

    send(client_socket, response.c_str(), response.size(), 0);
    close(client_socket);
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // Allow immediate reuse of the port
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Failed to bind\n";
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 128) < 0)
    {
        std::cerr << "Listen failed\n";
        close(server_fd);
        return 1;
    }

    std::cout << "TCP Server running on port 8080...\n";

    while (true)
    {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_socket = accept(server_fd, (sockaddr *)&client_addr, &client_len);
        if (client_socket < 0)
        {
            std::cerr << "Failed to accept connection\n";
            continue;
        }

        std::string ip = inet_ntoa(client_addr.sin_addr);

        // Fire a new thread per connection
        std::thread(handle_client, client_socket, ip).detach();
    }

    close(server_fd);
    return 0;
}
