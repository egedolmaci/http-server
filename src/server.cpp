
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <mutex>
#include <fstream>

std::mutex client_mutex;

typedef struct {
    int connect_fd;
    std::string dir;
} thread_args;

void *handle_client(void *args_ptr) {
    thread_args *args = (thread_args *) args_ptr;
    int connect_fd = args->connect_fd;
    std::string dir = args->dir;

    char buf[512];

    // Receive data from the client
    if (recv(connect_fd, buf, sizeof(buf), 0) < 0) {
        std::cerr << "Receiving failed\n";
        close(connect_fd);
        return nullptr;
    }

    std::string response;
    std::string msg(buf);

    // Log the received message
    std::clog << msg << "\n";

    if (msg.find("/files/") != std::string::npos) {
        // Extract file path from the request
        size_t start = msg.find("/files/") + 7; // Adjust to start after "/files/"
        size_t end = msg.find(' ', start);
        std::string file_to_retrieve = msg.substr(start, end - start);
        std::clog << "File to retrieve: " << file_to_retrieve << "\n";

        // Construct the full path and open the file
        std::string full_path = dir + file_to_retrieve;
        std::clog << "Full path: " << full_path << "\n";
        std::ifstream file(full_path, std::ios::binary | std::ios::ate);
        
        if (!file.is_open()) {
            response = "HTTP/1.1 404 Not Found\r\n\r\n";
        } else {
            // Get file size
            size_t size = file.tellg();
            file.seekg(0, std::ios::beg);

            // Read the file contents into a string
            std::string file_content(size, '\0');
            file.read(&file_content[0], size);

            // Create the response
            response = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: " + std::to_string(size) + "\r\n\r\n" + file_content;
        }
    } else if (msg.find("user-agent") != std::string::npos) {
        size_t start = msg.find("User-Agent: ") + 12;
        size_t end = msg.find("\r\n", start);
        std::string content_body = msg.substr(start, end - start);
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(content_body.length()) + "\r\n\r\n" + content_body;
    } else if (msg.find("/echo/") != std::string::npos) {
        size_t start = msg.find('/', 5);
        size_t end = msg.find(' ', start);
        std::string content_body = msg.substr(start + 1, end - start - 1);
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(content_body.length()) + "\r\n\r\n" + content_body;
    } else {
        response = "HTTP/1.1 404 Not Found\r\n\r\n";
    }

    // Send the response
    send(connect_fd, response.c_str(), response.size(), 0);

    // Clean up
    close(connect_fd);
    return nullptr;
}

int main(int argc, char **argv) {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
  

    std::string dir;
    if (argc == 3 && strcmp("--directory", argv[1]) == 0) {
            dir = argv[2];
    }

    std::cout << "Logs from your program will appear here!\n";

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "setsockopt failed\n";
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(4221);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        std::cerr << "Failed to bind to port 4221\n";
        return 1;
    }

    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        std::cerr << "listen failed\n";
        return 1;
    }

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    std::cout << "Waiting for a client to connect...\n";

    while (true) {
        int connect_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
        if (connect_fd < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        pthread_t thread_id;
        thread_args *args = new thread_args{connect_fd, dir};
        if (pthread_create(&thread_id, NULL, handle_client, (void*)args) < 0) {
            std::cerr << "Could not create thread\n";
            continue;
        }

        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}
