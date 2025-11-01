#ifndef RAM_H
#define RAM_H

#include "stack.h"

struct RAM
{
    int* data; // динамический массив ячеек памяти
    int size;  // размер используемой памяти
};

StackErr_t RAM_init (RAM* ram, int size);
StackErr_t RAM_read (const RAM* ram, unsigned address, int* value);
StackErr_t RAM_write(RAM* ram, unsigned address, int value);
StackErr_t RAM_clear(RAM* ram);
StackErr_t RAM_destroy(RAM* ram);

#endif // RAM_H