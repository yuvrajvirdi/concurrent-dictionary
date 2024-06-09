
#ifndef FILE_IO_H
#define FILE_IO_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void write_to_disk(const char *filename, const char *key, const char *value);
void read_from_disk(const char *filename, const char *key, char *value, size_t value_size);

#ifdef __cplusplus
}
#endif

#endif 
