#include "ipc.h"
#include "listener.h"
#include <gtk/gtk.h>
#include <msgpack.h>
#include <pthread.h>
#include <unistd.h>

static FancySocket *fs = NULL;
void handle_sigint(int sig) {
  (void)sig;
  if (fs) {
    fancy_socket_destroy(fs);
    printf("\nFancy exited like a boss.\n");
    fs = NULL;
  }
  _exit(0);
}

void *listen_thread(void *arg) {
  FancySocket *fs = (FancySocket *)arg;
  int status = fancy_socket_listener(fs);
  return NULL;
}

int main(int argc, char **argv) {
  signal(SIGINT, handle_sigint);
  fs = fancy_socket_create();
  int fsos = fancy_socket_open(fs);
  if (fsos < 0) {
    fprintf(stderr, "Failed to open fancy socket error code: %i", fsos);
    exit(1);
  }
  pthread_t thread;
  if (pthread_create(&thread, NULL, listen_thread, fs) != 0) {
    perror("pthread_create");
    return 1;
  };
  pthread_join(thread, NULL);

  return 0;
}
