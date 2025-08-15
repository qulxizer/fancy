#ifndef HANDLERS_H
#define HANDLERS_H
#include <msgpack/object.h>
typedef void *FancyHandler(const msgpack_object *obj, void *userdata);

void *fancy_handle_window(const msgpack_object *obj, void *userdata);
#endif
