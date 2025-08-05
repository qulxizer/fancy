#pragma once
struct {
  char *path;
  int sockfd;
} typedef FancySocket;

FancySocket *fancy_socket_create(void);
int fancy_socket_open(FancySocket *fs);
int fancy_socket_close(FancySocket *fs);
int fancy_socket_destroy(FancySocket *fs);
