#ifndef STRUCTS_H
#define STRUCTS_H

#include "stack.h"
#include "ram.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const int MAX_LINE_LENGTH          = 1000;
const int INITIAL_LABEL_CAPACITY   = 10;
const int INITIAL_COMMAND_CAPACITY = 1000;
const int MAX_CALL_STACK_SIZE      = 100;
const int DEFAULT_RAM_SIZE         = 1024;
const int DEFAULT_STACK_CAPACITY   = 100;

enum OperationCode
{
    OP_EXIT  = 0,
    OP_PUSH  = 1,
    OP_POP   = 2,
    OP_ADD   = 3,
    OP_SUB   = 4,
    OP_MUL   = 5,
    OP_DIV   = 6,
    OP_SQRT  = 7,
    OP_PRINT = 8,
    OP_PUSHR = 9,
    OP_POPR  = 10,
    OP_CALL  = 11,
    OP_RET   = 12,
    OP_PUSHH = 13,
    OP_POPH  = 14,
    OP_JMP   = 15,    // безусловный переход
    OP_JE    = 16,    // переход если равно
    OP_JNE   = 17,    // переход если не равно
    OP_JG    = 18,    // переход если больше
    OP_JL    = 19,    // переход если меньше
    OP_JGE   = 20,    // переход если больше или равно
    OP_JLE   = 21     // переход если меньше или равно
};

// структура для хранения метки
struct Label
{
    char name[MAX_LINE_LENGTH];
    int position;
};

// таблица меток с динамическим массивом
struct LabelTable
{
    Label* labels;
    int count;
    int capacity;
};

inline const char* GetRegisterName(Register_t reg)
{
    switch (reg)
    {
        case REG_RAX: return "A";
        case REG_RBX: return "B";
        case REG_RCX: return "C";
        case REG_RDX: return "D";
        case REG_REX: return "E";
        default: return "UNKNOWN";
    }
}

// инициализация и очистка таблицы меток
StackErr_t LabelTable_init(LabelTable* table);
StackErr_t LabelTable_add(LabelTable* table, const char* name, int position);
StackErr_t LabelTable_destroy(LabelTable* table);

// структура, которая передается процессору
struct Processor
{
    Stack_t stack;
    RAM ram;                // оперативная память
    int registers[5];       // RAX, RBX, RCX, RDX, REX
    int ip;                 // instruction pointer
    int callStack[100];     // стек вызовов для возврата из CALL
    int callStackSize;
};

// работа с регистрами
Register_t  ParseRegisterName(const char* reg_name);
const char* GetRegisterName(Register_t reg);
StackErr_t  execute_instruction(StackErr_t (*operation)(Stack_t*), Stack_t *stk, StackErr_t *err, const char* op_name);

// основные функции
int*       read_commands_from_file(const char* filename, int* commandCount);
StackErr_t execute_commands(int* commands, int commandCount);
void       write_commands_to_file(const char* filename, int* commands, int commandCount);

#endif // STRUCTS_H