#ifndef ENDIANNESS_H
#define ENDIANNESS_H

#include <stdint.h>
#include <stddef.h>

uint8_t _get_endianness(void);
void _swap_endian(void *p, size_t size);

#endif /* ENDIANNESS_H */
