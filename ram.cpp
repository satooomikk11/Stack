#include "ram.h"
#include "structs.h"
#include <stdio.h>

// инициализация RAM
StackErr_t RAM_init(RAM* ram)
{
    if (!ram) { return STACK_ERR_NULL_PTR; }
    
    for (int i = 0; i < RAM_SIZE; i++)
    {
        ram->data[i] = 0;
    }
    
    ram->size = RAM_SIZE;
    return STACK_OK;
}

// чтение из RAM по адресу
StackErr_t RAM_read(const RAM* ram, int address, int* value)
{
    if (!ram || !value) { return STACK_ERR_NULL_PTR; }
    
    if (address < 0 || address >= ram->size)
    {
        printf("Ошибка: чтение за пределами RAM (адрес %d)\n", address);
        return STACK_ERR_DAMAGED;
    }
    
    *value = ram->data[address];
    return STACK_OK;
}

// запись в RAM по адресу
StackErr_t RAM_write(RAM* ram, int address, int value)
{
    if (!ram) { return STACK_ERR_NULL_PTR; }
    
    if (address < 0 || address >= ram->size)
    {
        printf("Ошибка: запись за пределами RAM (адрес %d)\n", address);
        return STACK_ERR_DAMAGED;
    }
    
    ram->data[address] = value;
    return STACK_OK;
}

// очистка RAM
StackErr_t RAM_clear(RAM* ram)
{
    if (!ram) { return STACK_ERR_NULL_PTR; }
    
    for (int i = 0; i < ram->size; i++)
    {
        ram->data[i] = 0;
    }
    
    return STACK_OK;
}