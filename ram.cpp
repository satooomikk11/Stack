#include <stdio.h>
#include "ram.h"
#include "structs.h"

// инициализация RAM
StackErr_t RAM_init(RAM* ram, int size)
{
    if (!ram || size <= 0)
    { 
        return STACK_ERR_NULL_PTR; 
    }
    
    ram->data = (int*)calloc(size, sizeof(int));
    if (!ram->data)
    {
        return STACK_ERR_DAMAGED;
    }
    
    ram->size = size;
    return STACK_OK;
}

// чтение из RAM по адресу
StackErr_t RAM_read(const RAM* ram, unsigned address, int* value)
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
StackErr_t RAM_write(RAM* ram, unsigned address, int value)
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

// освобождение памяти RAM
StackErr_t RAM_destroy(RAM* ram)
{
    if (!ram) { return STACK_ERR_NULL_PTR; }
    
    if (ram->data) {
        free(ram->data);
        ram->data = NULL;
    }
    
    ram->size = 0;
    return STACK_OK;
}