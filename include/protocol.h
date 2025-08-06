#include <msgpack/object.h>
#include <stddef.h>
typedef enum {
  MSG_TYPE_EVENT,
  MSG_TYPE_COMMAND,
  MSG_TYPE_RESPONSE,
  MSG_TYPE_ERROR,
} MessageType;

typedef enum {
  EVENT_CLICKED,
  EVENT_HOVERED,
  EVENT_KEY_PRESSED,
  EVENT_KEY_RELEASED,
  EVENT_FOCUS_IN,
  EVENT_FOCUS_OUT,
} EventType;

size_t build_event(void *buf, size_t maxlen, const char *event_name,
                   const char *data);
size_t build_command(void *buf, size_t maxlen, const char *cmd,
                     msgpack_object *args);
size_t build_response(void *buf, size_t maxlen, int id, const char *result);
