#include "structs.h"

// инициализация таблицы меток
StackErr_t LabelTable_init(LabelTable* table)
{
    if (!table) return STACK_ERR_NULL_PTR;
    
    table->labels = (Label*)calloc(INITIAL_LABEL_CAPACITY, sizeof(Label));
    if (!table->labels) return STACK_ERR_DAMAGED;
    
    table->count = 0;
    table->capacity = INITIAL_LABEL_CAPACITY;
    return STACK_OK;
}

// добавление метки в таблицу
StackErr_t LabelTable_add(LabelTable* table, const char* name, int position)
{
    if (!table || !name) return STACK_ERR_NULL_PTR;
    
    // проверяем, нужно ли расширить массив
    if (table->count >= table->capacity)
    {
        int new_capacity = table->capacity * 2;
        Label* new_labels = (Label*)realloc(table->labels, new_capacity * sizeof(Label));
        if (!new_labels) return STACK_ERR_DAMAGED;
        
        table->labels = new_labels;
        table->capacity = new_capacity;
    }
    
    // добавляем новую метку
    strncpy(table->labels[table->count].name, name, MAX_LINE_LENGTH - 1);
    table->labels[table->count].position = position;
    table->count++;
    
    return STACK_OK;
}

// освобождение памяти таблицы меток
StackErr_t LabelTable_destroy(LabelTable* table)
{
    if (!table) return STACK_ERR_NULL_PTR;
    
    if (table->labels)
    {
        free(table->labels);
        table->labels = NULL;
    }
    
    table->count = 0;
    table->capacity = 0;
    return STACK_OK;
}