#ifndef _TT_SERIALIZER_H
#define _TT_SERIALIZER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    void (*write_buffer)(void* self_data, uint8_t* buffer, uint16_t size);
    void (*write_char)(void* self_data, uint8_t c);
    void (*print_dbg)(const char* str);
    void* data;
} tt_serializer_t;

typedef struct {
    uint16_t (*read_char)(void* self_data);
    bool (*eof)(void* self_data);
    void (*print_dbg)(const char* str);
    void* data;
} tt_deserializer_t;

#endif  // _TT_SERIALIZER_H