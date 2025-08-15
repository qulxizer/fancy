#ifndef UTILS_H

#include <msgpack/object.h>
typedef struct {
  const msgpack_object_kv *pairs;
  uint32_t size;
} FancyMap;

FancyMap fancy_map_from_msgpack(msgpack_object obj);
const msgpack_object *fancy_map_get(FancyMap *map, const char *key);

#endif // !UTILS_H
