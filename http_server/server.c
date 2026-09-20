#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "8080"

int main() {
  struct addrinfo hints;
  struct addrinfo *serverAddr = NULL;
  int sock = -1;
  int result = EXIT_FAILURE;

  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int status = getaddrinfo(NULL, PORT, &hints, &serverAddr);

  if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    goto cleanup;
  }

  sock = socket(serverAddr->ai_family, serverAddr->ai_socktype,
                serverAddr->ai_protocol);

  if (sock < 0) {
    perror("socket");
    goto cleanup;
  }

  int opt = 1;

  if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) < 0) {
    perror("setsocketopt");
    goto cleanup;
  }

  if ((bind(sock, serverAddr->ai_addr, serverAddr->ai_addrlen)) < 0) {
    perror("bind");
    goto cleanup;
  }

  if ((listen(sock, 10)) < 0) {
    perror("listen");
    goto cleanup;
  }

  printf("Server started on port %s\n", PORT);

  while (1) {
    int clientSock = accept(sock, NULL, NULL);

    if (clientSock < 0) {
      perror("accept");
      continue;
    }

    char buffer[4096];

    ssize_t bytes = recv(clientSock, buffer, sizeof(buffer) - 1, 0);

    if (bytes < 0) {
      perror("recv");
      close(clientSock);
      continue;
    }

    buffer[bytes] = '\0';

    char method[16];
    char path[256];
    char version[16];

    if (sscanf(buffer, "%15s %255s %15s", method, path, version) != 3) {
      fprintf(stderr, "bad Request");
      close(clientSock);
      continue;
    }

    puts(method);
    puts(path);
    puts(version);

    const char *body = NULL;

    char response[1024];

    if ((strcmp(path, "/")) == 0) {
      body = "Root page";
    }

    else if ((strcmp(path, "/hello")) == 0) {
      body = "Hqllo from C!";
    }

    else {
      body = "Error 404";
    }

    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             strlen(body), body);

    ssize_t sent = send(clientSock, response, strlen(response), 0);

    if (sent < 0) {
      perror("send");
    }

    close(clientSock);
  }

  result = EXIT_SUCCESS;

cleanup:
  if (serverAddr) {
    freeaddrinfo(serverAddr);
  }

  if (sock >= 0) {
    close(sock);
  }

  return result;
}
