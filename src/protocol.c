#include <msgpack/object.h>
#include <stddef.h>
size_t build_event(void *buf, size_t maxlen, const char *event_name,
                   const char *data) {

  return 0;
}

size_t build_command(void *buf, size_t maxlen, const char *cmd,
                     msgpack_object *args) {
  return 0;
}

size_t build_response(void *buf, size_t maxlen, int id, const char *result) {
  return 0;
}
