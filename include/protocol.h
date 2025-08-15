#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "gtk/gtk.h"
#include <msgpack/pack.h>
#include <stddef.h>

#define ENUM_GEN(ENUM) ENUM,
#define STRING_GEN(STRING) #STRING,

#define FOREACH_FANCY_MESSAGE_TYPE(FANCY_MESSAGE_TYPE)                         \
  FANCY_MESSAGE_TYPE(FANCY_MESSAGE_EVENT)                                      \
  FANCY_MESSAGE_TYPE(FANCY_MESSAGE_WINDOW)                                     \
  FANCY_MESSAGE_TYPE(FANCY_MESSAGE_COUNT)

typedef enum { FOREACH_FANCY_MESSAGE_TYPE(ENUM_GEN) } FancyMessageType;
extern const char *fancy_message_type_strs[];

#define FOREACH_FANCY_EVENT_TYPE(FANCY_EVENT_TYPE)                             \
  FANCY_EVENT_TYPE(FANCY_EVENT_CLICKED)                                        \
  FANCY_EVENT_TYPE(FANCY_EVENT_HOVERD)                                         \
  FANCY_EVENT_TYPE(FANCY_EVENT_KEY_PRESSED)                                    \
  FANCY_EVENT_TYPE(FANCY_EVENT_KEY_RELEASE)                                    \
  FANCY_EVENT_TYPE(FANCY_EVENT_FOCUS_IN)                                       \
  FANCY_EVENT_TYPE(FANCY_EVENT_FOCUS_OUT)                                      \
  FANCY_EVENT_TYPE(FANCY_EVENT_COUNT)

typedef enum { FOREACH_FANCY_EVENT_TYPE(ENUM_GEN) } FacnyEventType;
extern const char *fancy_event_type_strs[];

typedef struct {
  char *event_name;
  FacnyEventType event_type;
  char *widget_id;
  GtkWidget *widget;
} FancyEvent_t;

size_t fancy_encode_event(void *buf, size_t maxlen, const FancyEvent_t *event);

typedef struct {
  char *window_name;
  GtkApplication *app;
  GtkWindow *window;
} FancyWindow_t;

size_t fancy_decode_window(const char *buf, size_t maxlen, GtkApplication *app,
                           FancyWindow_t *window);

static size_t fancy_init_message(const FancyMessageType fmt,
                                 msgpack_packer *pk);
#endif
