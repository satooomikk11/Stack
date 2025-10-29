#ifndef RAM_H
#define RAM_H

#include "stack.h"

#define RAM_SIZE 1024

struct RAM
{
    int data[RAM_SIZE];  // массив ячеек памяти
    int size;            // размер используемой памяти
};

StackErr_t RAM_init (RAM* ram);
StackErr_t RAM_read (const RAM* ram, int address, int* value);
StackErr_t RAM_write(RAM* ram, int address, int value);
StackErr_t RAM_clear(RAM* ram);

#endif // RAM_H