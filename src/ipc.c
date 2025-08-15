#include "ipc.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

/**
 * fancy_socket_create:
 *
 * Allocates and initializes a FancySocket struct with a socket path
 * based on the current process ID and the XDG_RUNTIME_DIR environment variable.
 *
 * The socket path will be: "$XDG_RUNTIME_DIR/fancy-<pid>".
 *
 * Returns: Pointer to the newly allocated FancySocket on success,
 *          or NULL if XDG_RUNTIME_DIR is not set.
 *
 * Note: The caller is responsible for freeing the returned FancySocket
 *       and its path when no longer needed.
 */
FancySocket *fancy_socket_create(void) {
  const char *xdg = getenv("XDG_RUNTIME_DIR");
  if (!xdg) {
    fprintf(stderr, "Error: XDG_RUNTIME_DIR not set\n");
    return NULL;
  }

  FancySocket *fs = calloc(1, sizeof(FancySocket));
  if (!fs) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }

  pid_t pid = getpid();
  char pid_str[16];
  snprintf(pid_str, sizeof(pid_str), "%d", pid);

  // XDG_RUNTIME_DIR + / + fancy- + pid + null terminator
  size_t len = strlen(xdg) + 1 + strlen("fancy-") + strlen(pid_str) + 1;
  fs->path = malloc(len);
  snprintf(fs->path, len, "%s/fancy-%s", xdg, pid_str);

  return fs;
}

/**
 * fancy_socket_open:
 * @fs: Pointer to a FancySocket struct with a valid socket path.
 *
 * Creates a Unix domain socket, binds it to the path specified in @fs,
 * and starts listening for incoming connections.
 *
 * Returns: 0 on success, or -1 on failure (with errno set).
 *          On failure, the socket file descriptor is closed and not stored.
 */
int fancy_socket_open(FancySocket *fs) {
  if (!fs) {
    return -1;
  }
  int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    return -1;
  }
  fs->sockfd = sockfd;

  struct sockaddr_un addr = {0};
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, fs->path, sizeof(addr.sun_path) - 1);

  unlink(fs->path); // remove if already exists

  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    close(sockfd);
    return -1;
  }

  if (listen(sockfd, 5) < 0) {
    perror("listen");
    close(sockfd);
    return -1;
  }
  int flags = fcntl(sockfd, F_GETFL, 0);
  if (flags < 0) {

    perror("fcntl F_GETFL");
    return -1;
  }
  if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)) {
    perror("fcntl F_SETFL");
    return -1;
  }

  return 0;
}

/**
 * fancy_socket_close:
 * @fs: Pointer to a FancySocket instance.
 *
 * Closes the socket file descriptor if it's open.
 * On failure, prints an error message and returns -1.
 * Always sets the file descriptor to -1 after closing.
 *
 * Returns: 0 on success, -1 on failure.
 */
int fancy_socket_close(FancySocket *fs) {
  if (!fs)
    return -1;

  if (fs->sockfd <= 0) {
    return -1;
  }
  if (close(fs->sockfd) < 0) {
    perror("close");
    return -1;
  }
  fs->sockfd = -1;
  return 0;
}

/**
 * fancy_socket_destroy:
 * @fs: Pointer to a FancySocket instance.
 *
 * Unlinks the socket file, frees the socket path, and deallocates
 * the FancySocket struct. Safe to call multiple times; does nothing if @fs is
 * NULL.
 *
 * Returns: 0 on success, -1 ib failure.
 */
int fancy_socket_destroy(FancySocket *fs) {
  if (!fs)
    return -1;
  unlink(fs->path);
  free(fs->path);
  free(fs);
  return 0;
}

/**
 * fancy_socket_accept:
 * @fs: Pointer to a FancySocket.
 *
 * Non-blocking accept on @fs->sockfd.
 *
 * Returns:
 * - The new client file descriptor on success (> 0).
 * - 0 if no pending connection is ready (errno is EAGAIN or EWOULDBLOCK).
 * - -1 on a fatal error (after calling perror).
 */
int fancy_socket_accept(FancySocket *fs) {
  if (!fs || fs->sockfd < 0)
    return -1;

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd =
      accept(fs->sockfd, (struct sockaddr *)&client_addr, &client_len);

  if (client_fd < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
      return 0;
    }
    perror("accept");
    return -1;
  }

  return client_fd;
}
