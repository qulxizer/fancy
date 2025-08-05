#include "ipc.h"
#include "sys/epoll.h"
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int fancy_socket_listener(FancySocket *fs) {
  int epfd = epoll_create1(0);
  if (epfd < 0) {
    perror("epoll_create1");
    return -1;
  }
  struct epoll_event ev = {
      .events = EPOLLIN,
      .data.fd = fs->sockfd,
  };
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, fs->sockfd, &ev) < 0) {
    perror("epoll_ctl ADD");
    return -1;
  }

  struct epoll_event events[1];
  int len = sizeof(events) / sizeof(struct epoll_event);
  while (true) {
    int n = epoll_wait(epfd, events, len, -1);
    if (n < 0) {
      fprintf(stderr, "fuck you \n");
      continue;
    }
    for (int i = 0; i < n; i++) {
      if (events[i].data.fd != fs->sockfd) {
        continue;
      }
      int client_fd = accept(fs->sockfd, NULL, NULL);
      if (client_fd < 0) {
        perror("accept");
        continue;
      }

      char buf[128];
      int len = read(client_fd, buf, sizeof(buf) - 1);
      if (len > 0) {
        buf[len] = 0;
        printf("You typed: %s \n", buf);
      }
    }
  }
  return 0;
}
