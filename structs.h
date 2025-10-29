#ifndef STRUCTS_H
#define STRUCTS_H

#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_LINE_LENGTH 1000

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
    OP_RET   = 12
};

// структура для хранения метки
struct Label
{
    char name[MAX_LINE_LENGTH];
    int position;
};

// таблица меток
struct LabelTable
{
    Label labels[10];
    int count;
};

// структура, которая передается процессору
struct Processor
{
    Stack_t stack;
    int registers[4];   // RAX, RBX, RCX, RDX
    int ip;             // instruction pointer
    int callStack[100]; // стек вызовов для возврата из CALL
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