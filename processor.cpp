#include <math.h>

#include "structs.h"
#include "ram.h"
#include "processor.h"

// макрос для обработки команд с одним аргументом
#define PROCESS_ONE_ARG_COMMAND(opcode, process_func, error_msg)                    \
    case opcode: {                                                                  \
        if (proc.ip + 1 < commandCount)                                             \
        {                                                                           \
            process_func;                                                           \
            proc.ip++;                                                              \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            printf("ERROR: " error_msg "\n");                                       \
        }                                                                           \
        break;                                                                      \
    }

// макрос для обработки команд без аргументов
#define PROCESS_NO_ARG_COMMAND(opcode, process_func)                                \
    case opcode: {                                                                  \
        process_func;                                                               \
        break;                                                                      \
    }

// макрос для обработки условных переходов
#define CASE_COND_JMP_COMMAND(opcode, op_name, condition)                           \
    case opcode:                                                                    \
    {                                                                               \
        if (proc.ip + 1 >= commandCount)                                            \
        {                                                                           \
            printf("ERROR: нет адреса после " op_name "\n");                        \
            return STACK_ERR_DAMAGED;                                               \
        }                                                                           \
        StackErr_t err1 = STACK_OK, err2 = STACK_OK;                                \
        int b = StackPop(&proc.stack, &err1);                                       \
        int a = StackPop(&proc.stack, &err2);                                       \
        if (err1 != STACK_OK || err2 != STACK_OK)                                   \
        {                                                                           \
            printf("Ошибка " op_name ": невозможно извлечь значения из стека\n");   \
            return STACK_ERR_DAMAGED;                                               \
        }                                                                           \
        if (a condition b)                                                          \
        {                                                                           \
            int target = commands[proc.ip + 1];                                     \
            if (!(0 <= target && target < commandCount))                            \
            {                                                                       \
                printf("Ошибка " op_name ": неверный адрес %d\n", target);          \
                return STACK_ERR_DAMAGED;                                           \
            }                                                                       \
            printf(op_name ": %d " #condition " %d, переход к %d\n", a, b, target); \
            proc.ip = target;                                                       \
            break;                                                                  \
        }                                                                           \
        printf(op_name ": %d !" #condition " %d, переход не выполнен\n", a, b);     \
        proc.ip++;                                                                  \
        break;                                                                      \
    }

// сложение верхних элементов стека
StackErr_t StackAdd(Stack_t *stk)
{
    if (!stk) return STACK_ERR_NULL_PTR;
    
    StackErr_t err = STACK_OK;
    int b = StackPop(stk, &err);
    if (err != STACK_OK) return err;
    
    int a = StackPop(stk, &err);
    if (err != STACK_OK)
    {
        StackPush(stk, b);
        return err;
    }
    
    int result = a + b;
    return StackPush(stk, result);
}

// вычитание верхних элементов стека
StackErr_t StackSub(Stack_t *stk)
{
    if (!stk) return STACK_ERR_NULL_PTR;
    
    StackErr_t err = STACK_OK;
    int b = StackPop(stk, &err);
    if (err != STACK_OK) return err;
    
    int a = StackPop(stk, &err);
    if (err != STACK_OK)
    {
        StackPush(stk, b);
        return err;
    }
    
    int result = a - b;
    return StackPush(stk, result);
}

// умножение верхних элементов стека
StackErr_t StackMul(Stack_t *stk)
{
    // StackDump(stk);
    if (!stk) return STACK_ERR_NULL_PTR;
    
    StackErr_t err = STACK_OK;
    int b = StackPop(stk, &err);
    if (err != STACK_OK) return err;
    
    int a = StackPop(stk, &err);
    if (err != STACK_OK)
    {
        StackPush(stk, b);
        return err;
    }
    
    int result = a * b;
    return StackPush(stk, result);
}

// деление верхних элементов стека
StackErr_t StackDiv(Stack_t *stk)
{
    if (!stk) return STACK_ERR_NULL_PTR;
    
    StackErr_t err = STACK_OK;
    int b = StackPop(stk, &err);
    if (err != STACK_OK) return err;
    
    if (b == 0)
    {
        StackPush(stk, b);
        printf("Ошибка: деление на ноль\n");
        return STACK_ERR_DAMAGED;
    }

    int a = StackPop(stk, &err);
    if (err != STACK_OK)
    {
        StackPush(stk, b);
        return err;
    }

    int result = a / b;
    return StackPush(stk, result);
}

// корень из верхнего элемента стека
StackErr_t StackSqrt(Stack_t *stk)
{
    if (!stk) return STACK_ERR_NULL_PTR;
    
    StackErr_t err = STACK_OK;
    int a = StackPop(stk, &err);
    if (err != STACK_OK) return err;

    if (a < 0)
    {
        StackPush(stk, a);
        printf("Ошибка: корень из отрицательного числа\n");
        return STACK_ERR_DAMAGED;
    }

    int result = sqrt(a);
    return StackPush(stk, result);
}

// вывод верхнего элемента стека
StackErr_t StackPrint(Stack_t *stk)
{
    if (!stk) return STACK_ERR_NULL_PTR;
    
    StackErr_t err = StackVerify(stk);
    if (err != STACK_OK) return err;

    if (stk->size == 0)
    {
        printf("Стек пуст\n");
        return STACK_OK;
    }

    printf("Верхний элемент: %d\n", stk->data[stk->size - 1]);
    return STACK_OK;
}

// загрузка из RAM в стек
StackErr_t StackPushH(Stack_t *stk, RAM* ram)
{
    if (!stk || !ram) return STACK_ERR_NULL_PTR;
    
    StackErr_t err = STACK_OK;
    int address = StackPop(stk, &err);
    if (err != STACK_OK) return err;
    
    int value = 0;
    err = RAM_read(ram, address, &value);
    if (err != STACK_OK) return err;
    
    return StackPush(stk, value);
}

// сохранение из стека в RAM
StackErr_t StackPopH(Stack_t *stk, RAM* ram)
{
    if (!stk || !ram) return STACK_ERR_NULL_PTR;
    
    StackErr_t err = STACK_OK;
    int value = StackPop(stk, &err);
    if (err != STACK_OK) return err;
    // -svg
    int address = StackPop(stk, &err);
    if (err != STACK_OK)
    {
        StackPush(stk, value);
        return err;
    }
    
    return RAM_write(ram, address, value);
}

// функция выполнения (вместо копипаста)
StackErr_t execute_instruction(StackErr_t (*operation)(Stack_t*), Stack_t *stk, StackErr_t *err, const char* op_name)
{    
    StackErr_t result = operation(stk);
    
    if (err != NULL) { *err = result; }
    
    if (result == STACK_OK) { printf("Команда %s выполнена успешно\n", op_name); }
    else { printf("Ошибка выполнения %s: %d\n", op_name, result); }
    
    return result;
}

// функция выполнения для операций с RAM
StackErr_t execute_instruction_ram(StackErr_t (*operation)(Stack_t*, RAM*), Stack_t *stk, RAM* ram, StackErr_t *err, const char* op_name)
{
    StackErr_t result = operation(stk, ram);
    
    if (err != NULL) { *err = result; }
    
    if (result == STACK_OK) { printf("Команда %s выполнена успешно\n", op_name); }
    else { printf("Ошибка выполнения %s: %d\n", op_name, result); }
    
    return result;
}

// инициализация структуры для процессора
StackErr_t processor_init(Processor* proc, unsigned stack_capacity)
{
    if (!proc) { return STACK_ERR_NULL_PTR; }
    
    // инициализация стека
    StackErr_t err = StackInit(&proc->stack, stack_capacity);
    if (err != STACK_OK) { return err; }
    
    // инициализация RAM
    err = RAM_init(&proc->ram, DEFAULT_RAM_SIZE);
    if (err != STACK_OK)
    { 
        StackDestroy(&proc->stack);
        return err; 
    }
    
    // инициализация регистров
    for (int i = 0; i < 5; i++)
    {
        proc->registers[i] = 0;
    }
    
    // инициализация указателя на команды
    proc->ip = 0;

    // инициализация стека вызовов
    proc->callStackSize = 0;
    
    return STACK_OK;
}

StackErr_t StackCall(int* commands, int commandCount, Processor* proc, int target_position)
{
    if (!commands || !proc)
    {
        return STACK_ERR_NULL_PTR;
    }
    
    // проверяем адрес метки
    if (target_position < 0 || target_position >= commandCount)
    {
        printf("Ошибка CALL: неверный адрес метки %d\n", target_position);
        return STACK_ERR_DAMAGED;
    }
    
    // сохраняем адрес возврата (позицию после CALL команды)
    if (proc->callStackSize < MAX_CALL_STACK_SIZE)
    {
        proc->callStack[proc->callStackSize++] = proc->ip + 2;
        printf("CALL: сохранен адрес возврата %d\n", proc->ip + 2);
    }
    else
    {
        printf("Ошибка: переполнение стека вызовов\n");
        return STACK_ERR_OVERFLOW;
    }
    
    // переходим к ассемблерной функции
    printf("CALL: переход к адресу %d\n", target_position);
    proc->ip = target_position;
    
    return STACK_OK;
}

StackErr_t StackRet(Processor* proc)
{
    if (!proc)
    {
        return STACK_ERR_NULL_PTR;
    }
    
    if (proc->callStackSize == 0)
    {
        printf("Ошибка RET: стек вызовов пуст\n");
        return STACK_ERR_EMPTY;
    }
    
    // восстанавливаем адрес возврата из стека вызовов
    int return_address = proc->callStack[--proc->callStackSize];
    printf("RET: возврат к адресу %d\n", return_address - 1);
    proc->ip = return_address - 1;
    
    return STACK_OK;
}

// выполнение команд из массива (процессор)
StackErr_t execute_commands(int* commands, int commandCount)
{
    // создаём процессор
    Processor proc;
    StackErr_t err = processor_init(&proc, DEFAULT_STACK_CAPACITY);
    if (err != STACK_OK)
    {
        printf("Ошибка инициализации процессора: %d\n", err);
        return err;
    }
    
    proc.ip = 0;
    
    while (proc.ip < commandCount)
    {
        int opcode = commands[proc.ip];
        switch (opcode)
        {
            CASE_COND_JMP_COMMAND(OP_JE,  "JE",  ==)
            CASE_COND_JMP_COMMAND(OP_JNE, "JNE", !=)
            CASE_COND_JMP_COMMAND(OP_JG,  "JG",  > )
            CASE_COND_JMP_COMMAND(OP_JL,  "JL",  < )
            CASE_COND_JMP_COMMAND(OP_JGE, "JGE", >=)
            CASE_COND_JMP_COMMAND(OP_JLE, "JLE", <=)

            PROCESS_ONE_ARG_COMMAND(OP_PUSHR, 
                Register_t reg = (Register_t)commands[proc.ip + 1];
                err = StackPushReg(&proc.stack, reg);
                if (err != STACK_OK) 
                {
                    printf("PUSHR ERROR: %d\n", err);
                }
            , "нет регистра после PUSHR")
            
            PROCESS_ONE_ARG_COMMAND(OP_POPR,
                Register_t reg = (Register_t)commands[proc.ip + 1];
                err = StackPopReg(&proc.stack, reg);
                if (err != STACK_OK)
                {
                    printf("POPR ERROR: %d\n", err);
                }
            , "нет регистра после POPR")
            
            PROCESS_ONE_ARG_COMMAND(OP_CALL,
                int target_position = commands[proc.ip + 1];
                err = StackCall(commands, commandCount, &proc, target_position);
                if (err != STACK_OK)
                {
                    printf("CALL ERROR: %d\n", err);
                    proc.ip++;
                }
            , "нет целевой позиции после CALL")
            
            PROCESS_ONE_ARG_COMMAND(OP_PUSH,
                int value = commands[proc.ip + 1];
                err = StackPush(&proc.stack, value);
                if (err == STACK_OK)
                {
                    printf("Добавлено: %d\n", value);
                }
                else
                {
                    printf("Ошибка добавления: %d\n", err);
                }
            , "нет числа после PUSH")
            
            PROCESS_NO_ARG_COMMAND(OP_RET,
                err = StackRet(&proc);
                if (err != STACK_OK)
                {
                    printf("RET ERROR: %d\n", err);
                }
            )
            
            PROCESS_NO_ARG_COMMAND(OP_PUSHH,
                execute_instruction_ram(StackPushH, &proc.stack, &proc.ram, &err, "PUSHH");
            )
            
            PROCESS_NO_ARG_COMMAND(OP_POPH,
                execute_instruction_ram(StackPopH, &proc.stack, &proc.ram, &err, "POPH");
            )
            
            PROCESS_NO_ARG_COMMAND(OP_EXIT,
                StackDestroy(&proc.stack);
                RAM_destroy(&proc.ram);
                printf("Выход из программы\n");
                return STACK_OK;
            )
            
            PROCESS_NO_ARG_COMMAND(OP_POP,
                StackErr_t pop_err = STACK_OK;
                int popped = StackPop(&proc.stack, &pop_err);
                if (pop_err == STACK_OK)
                {
                    printf("Извлечено: %d\n", popped);
                }
                else
                {
                    printf("Ошибка извлечения: %d\n", pop_err);
                }
            )
            
            PROCESS_NO_ARG_COMMAND(OP_ADD,
                execute_instruction(StackAdd, &proc.stack, &err, "ADD");
            )
            
            PROCESS_NO_ARG_COMMAND(OP_SUB,
                execute_instruction(StackSub, &proc.stack, &err, "SUB");
            )
            
            PROCESS_NO_ARG_COMMAND(OP_MUL,
                execute_instruction(StackMul, &proc.stack, &err, "MUL");
            )
            
            PROCESS_NO_ARG_COMMAND(OP_DIV,
                execute_instruction(StackDiv, &proc.stack, &err, "DIV");
            )
            
            PROCESS_NO_ARG_COMMAND(OP_SQRT,
                execute_instruction(StackSqrt, &proc.stack, &err, "SQRT");
            )
            
            PROCESS_NO_ARG_COMMAND(OP_PRINT,
                execute_instruction(StackPrint, &proc.stack, &err, "PRINT");
            )
                   
            default:
                printf("Неизвестный код операции: %d\n", opcode);
                break;
        }

        proc.ip++;
    }
    
    StackDestroy(&proc.stack);
    RAM_destroy(&proc.ram);
    return STACK_OK;
}