#include <msgpack/object.h>
#include <stdio.h>
void *fancy_handle_window(const msgpack_object *obj, void *userdata) {
  printf("KIKI do you love me.\n");
  return NULL;
}
