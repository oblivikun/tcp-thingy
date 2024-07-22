#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

// Global variables
int active_connections = 0;
pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread function to handle client connections
void *handle_client(void *arg) {
  int client_sock = *(int *)arg;
  char buffer[1024] = {0};

  // Increment active connections count safely
  pthread_mutex_lock(&connection_mutex);
  active_connections++;
  pthread_mutex_unlock(&connection_mutex);

  // Receive and send data to/from client
  read(client_sock, buffer, sizeof(buffer));
  printf("Received: %s\n", buffer);
  write(client_sock, "Message received", 16);

  // Decrement active connections count safely
  pthread_mutex_lock(&connection_mutex);
  active_connections--;
  pthread_mutex_unlock(&connection_mutex);

  close(client_sock);
  return NULL;
}

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  pthread_t thread_id;

  // Create socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Attach socket to port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Bind socket to port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  printf("Listener on port %d \n", PORT);

  // Listen for connections
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  printf("Waiting for connections...\n");

  while (1) {
    // Accept connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    printf("Connection accepted\n");

    pthread_create(&thread_id, NULL, handle_client, (void *)&new_socket);
    pthread_detach(thread_id);

    // Print active connections for demonstration


  }
  return 0;
}
