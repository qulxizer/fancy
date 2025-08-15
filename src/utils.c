#include <msgpack/object.h>
#include <stdio.h>
#include <string.h>
typedef struct {
  const msgpack_object_kv *pairs;
  uint32_t size;
} FancyMap;

FancyMap fancy_map_from_msgpack(msgpack_object obj) {
  FancyMap map = {0};
  if (obj.type == MSGPACK_OBJECT_MAP) {
    map.pairs = obj.via.map.ptr;
    map.size = obj.via.map.size;
  }
  return map;
}

const msgpack_object *fancy_map_get(FancyMap *map, const char *key) {
  for (uint32_t i = 0; i < map->size; i++) {
    const msgpack_object *k = &map->pairs[i].key;
    if (k->type == MSGPACK_OBJECT_STR &&
        strncmp(k->via.str.ptr, key, k->via.str.size) == 0 &&
        strlen(key) == k->via.str.size) {
      return &map->pairs[i].val;
    }
  }
  return NULL;
}
