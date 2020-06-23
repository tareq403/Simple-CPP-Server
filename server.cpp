#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

#define PORT 9999
#define MAX_CONNECTION 10
#define MESSAGE_MAX_SIZE 100

int main() {
    // Create socket (IPv4, TCP)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cout << "Failed to create socket. Errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // Listen to port at any address
    sockaddr_in sockaddrIn;
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_addr.s_addr = INADDR_ANY;
    sockaddrIn.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) &sockaddrIn, sizeof(sockaddrIn)) < 0) {
        std::cout << "Failed to bind to port " << PORT << ". Errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(sockfd, MAX_CONNECTION) < 0) {
        std::cout << "Failed to listen to port " << PORT << ". Errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // GRAB a connection from the queue
    auto addrlen = sizeof(sockaddrIn);
    int connection = accept(sockfd, (struct sockaddr *) &sockaddrIn, (socklen_t *) &addrlen);
    if (connection < 0) {
        std::cout << "Failed to grab connection. Errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read from the connection
        char buffer[MESSAGE_MAX_SIZE];
        auto bytesRead = read(connection, buffer, MESSAGE_MAX_SIZE);
        std::cout << "Received message: " << buffer << std::endl;

        // Send response
        std::string response = "Message received.\n";
        send(connection, response.c_str(), response.size(), 0);

        // Check if message is equals to "EXIT" command
        std::string inputMessage(buffer);
        std::string terminate = "EXIT";
        if (inputMessage.compare(terminate) == 0) {
            break;
        }
    }

    // Close connections
    close(connection);
    close(sockfd);
    return 0;
}
