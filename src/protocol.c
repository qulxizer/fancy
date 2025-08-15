#include "protocol.h"
#include "gtk/gtk.h"
#include "utils.h"
#include <msgpack/object.h>
#include <msgpack/pack.h>
#include <msgpack/sbuffer.h>
#include <msgpack/unpack.h>
#include <stddef.h>
#include <stdio.h>

const char *fancy_event_type_strs[] = {FOREACH_FANCY_EVENT_TYPE(STRING_GEN)};

const char *fancy_message_type_strs[] = {
    FOREACH_FANCY_MESSAGE_TYPE(STRING_GEN)};

/**
 * fancy_init_message:
 * @fmt: Message type.
 * @pk: msgpack_packer bound to a msgpack_sbuffer.
 *
 * Packs a 2-entry map: "type" -> @fmt, "data" -> (value to be packed later).
 *
 * Returns: Bytes written to the buffer.
 */
static size_t fancy_init_message(const FancyMessageType fmt,
                                 msgpack_packer *pk) {
  msgpack_sbuffer *sbuf = (msgpack_sbuffer *)pk->data;
  size_t before = sbuf->size;

  msgpack_pack_map(pk, 2);

  msgpack_pack_str(pk, 4);
  msgpack_pack_str_body(pk, "type", 4);
  msgpack_pack_int(pk, fmt);

  msgpack_pack_str(pk, 4);
  msgpack_pack_str_body(pk, "data", 4);

  return sbuf->size - before;
}

/**
 * fancy_encode_event:
 * @buf: Output buffer.
 * @maxlen: Size of @buf.
 * @event: Event data to encode.
 *
 * Encodes a FancyEvent as a MessagePack map inside a MESSAGE_EVENT message.
 * Copies the encoded bytes into @buf if they fit.
 *
 * Returns: Total bytes encoded (may be > @maxlen if truncated).
 */
size_t fancy_encode_event(void *buf, size_t maxlen, const FancyEvent_t *event) {
  msgpack_sbuffer *buffer = msgpack_sbuffer_new();
  msgpack_packer *pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);
  size_t written = fancy_init_message(FANCY_MESSAGE_EVENT, pk);
  msgpack_pack_map(pk, 3);

  msgpack_pack_str(pk, 11);
  msgpack_pack_str_body(pk, "event_name", 11);
  int event_name_len = strlen(event->event_name);
  msgpack_pack_str(pk, event_name_len);
  msgpack_pack_str_body(pk, event->event_name, event_name_len);

  msgpack_pack_str(pk, 11);
  msgpack_pack_str_body(pk, "event_type", 11);
  msgpack_pack_int(pk, event->event_type);

  msgpack_pack_str(pk, 10);
  msgpack_pack_str_body(pk, "widget_id", 10);
  int widget_id_len = strlen(event->widget_id);
  msgpack_pack_str(pk, widget_id_len);
  msgpack_pack_str_body(pk, event->widget_id, widget_id_len);

  written = buffer->size;
  if (written <= maxlen) {
    memcpy(buf, buffer->data, written);
  }
  msgpack_packer_free(pk);
  msgpack_sbuffer_free(buffer);

  return written;
};

size_t fancy_decode_window(const char *buf, size_t maxlen, GtkApplication *app,
                           FancyWindow_t *window) {

  msgpack_unpacked msg;
  msgpack_unpacked_init(&msg);

  if (!msgpack_unpack_next(&msg, buf, maxlen, NULL)) {
    msgpack_unpacked_destroy(&msg);
    return -1;
  }
  msgpack_object root = msg.data;
  FancyMap map = fancy_map_from_msgpack(root);
  const msgpack_object *window_name = fancy_map_get(&map, "type");
  if (window_name->type != MSGPACK_OBJECT_STR) {
    return -1;
  }
  const char *window_name_str = window_name->via.str.ptr;

  return 0;
};
