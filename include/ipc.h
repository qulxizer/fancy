#ifndef IPC_H
#define IPC_H

typedef struct {
  char *path;
  int sockfd;
} FancySocket;

FancySocket *fancy_socket_create(void);
int fancy_socket_open(FancySocket *fs);
int fancy_socket_close(FancySocket *fs);
int fancy_socket_destroy(FancySocket *fs);
int fancy_socket_accept(FancySocket *fs);

#endif
