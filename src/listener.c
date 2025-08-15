#include "handlers.h"
#include "ipc.h"
#include "logger.h"
#include "protocol.h"
#include "sys/epoll.h"
#include "utils.h"
#include <msgpack/object.h>
#include <msgpack/unpack.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int setup_epoll(FancySocket *fs) {
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
  return epfd;
}

// returns -1 if type not found
static FancyMessageType decodeMessageType(const char *buf,
                                          const unsigned int bufsize,
                                          msgpack_object *data) {
  msgpack_unpacked msg;
  msgpack_unpacked_init(&msg);

  if (!msgpack_unpack_next(&msg, buf, bufsize, 0)) {
    loggy_error(&lgy, "failed to unpack message pack message");
    msgpack_unpacked_destroy(&msg);
    return -1;
  }

  FancyMap map = fancy_map_from_msgpack(msg.data);
  const msgpack_object *mdata = fancy_map_get(&map, "data");
  *data = *mdata;

  const msgpack_object *type = fancy_map_get(&map, "type");
  if (type->type != MSGPACK_OBJECT_STR) {
    loggy_error(&lgy, "message pack field: type is not int, want: %i, got: %i",
                MSGPACK_OBJECT_STR, type->type);
    return -1;
  }

  char type_str[type->via.str.size + 1];
  memcpy(type_str, type->via.str.ptr, type->via.str.size);
  type_str[type->via.str.size] = '\0';
  for (int i = 0; i < FANCY_MESSAGE_COUNT; i++) {
    if (strcmp(type_str, fancy_message_type_strs[i]) == 0) {
      return i;
    }
  }
  loggy_warn(&lgy, "unknown message type: %s", type_str);
  return -1;
}

int fancy_socket_listener(FancySocket *fs) {
  int epfd = setup_epoll(fs);
  if (epfd < 0) {
    return -1;
  }

  struct epoll_event events[1];
  int len = sizeof(events) / sizeof(struct epoll_event);
  while (true) {
    int n = epoll_wait(epfd, events, len, -1);
    if (n < 0) {
      perror("epoll_wait");
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

      char buf[65536];

      size_t r = read(client_fd, buf, 65536);
      msgpack_object data;
      switch (decodeMessageType(buf, sizeof(buf), &data)) {
      case FANCY_MESSAGE_WINDOW:
        fancy_handle_window(&data, NULL);
        break;
      default:
        break;
      }
    }
  }
  return 0;
}
