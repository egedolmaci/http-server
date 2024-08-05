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

std::mutex client_mutex;

void *handle_client(void *connect_fd_ptr) {


  client_mutex.lock();
  int connect_fd = *(int*)connect_fd_ptr;
  char buf[512];

  if (recv(connect_fd, buf, sizeof buf, 0) < 0) {
    std::cerr << "recieving failed\n";
  }

  
  std::string response = ""; 
  std::string msg(buf);

  pthread_t thread_id = pthread_self();
  std::clog << thread_id << "\n" << msg << "\n";
  
  if (msg[4] == '/' && msg[5] == ' ') {
    response = "HTTP/1.1 200 OK\r\n\r\n";
  } else if (msg.find("user-agent") != std::string::npos) {
    size_t start = msg.find("User-Agent: ");
    start += 12;
    size_t end = msg.find("\r\n", start);
    std::clog << start << "\n" << end << "\n";
    std::string content_body = msg.substr(start, end - start);
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(content_body.length()) + "\r\n\r\n" + content_body;

  } else if (msg.find("/echo/") != std::string::npos) {
    size_t start = msg.find('/', 5);
    size_t end = msg.find(' ', start);
    std::string content_body = msg.substr(start+1,end - start - 1);
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(content_body.length()) + "\r\n\r\n" + content_body;
  } else {
    response = "HTTP/1.1 404 Not Found\r\n\r\n";
  }

  client_mutex.unlock();
  send(connect_fd, response.c_str(), response.size(), 0);
  delete (int*)connect_fd_ptr;
  return NULL;
}
  

int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment this block to pass the first stage
  //
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   std::cerr << "Failed to create server socket\n";
   return 1;
  }
 
  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
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
 
 
  int connect_fd;
  while (true) {

    if ((connect_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len)) < 0) {
      std::cerr << "Accept failed\n";
      continue;
    } 

    pthread_t thread_id;
    int *connect_fd_ptr = new int;
    *connect_fd_ptr = connect_fd;
    if (pthread_create(&thread_id, NULL, handle_client, (void*)connect_fd_ptr) < 0) {
      std::cerr << "Could not create thread\n";
      continue;
    }

    pthread_detach(thread_id);

  }

  close(connect_fd);
  close(server_fd);
  return 0;
}
