#include "calculator.h"
#include <string.h>

const int MAX_LINE_LENGTH = 1000;

enum OperationCode
{
    OP_EXIT  = 0,
    OP_PUSH  = 1,
    OP_POP   = 2,
    OP_ADD   = 3,
    OP_SUB   = 4,
    OP_MUL   = 5,
    OP_DIV   = 6,
    OP_PRINT = 7,
    OP_PUSHR = 8,
    OP_POPR  = 9
};

// сложение верхних элементов стека
StackErr_t StackAdd(Stack_t *stk)
{
    if (!stk)
    {
        return STACK_ERR_NULL_PTR;
    }
    StackErr_t err;

    int b = StackPop(stk, &err);
    int a = StackPop(stk, &err);

    int result = a + b;
    return StackPush(stk, result);
}

// вычитание верхних элементов стека
StackErr_t StackSub(Stack_t *stk)
{
    if (!stk)
    {
        return STACK_ERR_NULL_PTR;
    }
    StackErr_t err;

    int b = StackPop(stk, &err);
    int a = StackPop(stk, &err);

    int result = a - b;
    return StackPush(stk, result);
}

// умножение верхних элементов стека
StackErr_t StackMul(Stack_t *stk)
{
    if (!stk)
    {
        return STACK_ERR_NULL_PTR;
    }
    StackErr_t err;

    int b = StackPop(stk, &err);
    int a = StackPop(stk, &err);
    
    int result = a * b;
    return StackPush(stk, result);
}

// деление верхних элементов стека
StackErr_t StackDiv(Stack_t *stk)
{
    if (!stk)
    {
        return STACK_ERR_NULL_PTR;
    }
    StackErr_t err;

    int b = StackPop(stk, &err);
    if (b == 0)
    {
        StackPush(stk, b);
        printf("Ошибка: деление на ноль\n");
        return STACK_ERR_DAMAGED;
    }

    int a = StackPop(stk, &err);

    int result = a / b;
    return StackPush(stk, result);
}

// вывод верхнего элемента стека
StackErr_t StackPrint(Stack_t *stk)
{
    if (!stk)
    {
        return STACK_ERR_NULL_PTR;
    }
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

Register_t ParseRegisterName(const char* reg_name)
{
    assert(reg_name);

    if (strcmp(reg_name, "RAX") == 0) return REG_RAX;
    if (strcmp(reg_name, "RBX") == 0) return REG_RBX;
    if (strcmp(reg_name, "RCX") == 0) return REG_RCX;
    if (strcmp(reg_name, "RDX") == 0) return REG_RDX;
    return (Register_t)-1; // Неверный регистр
}

// чтение из файла и преобразование в массив
int* read_commands_from_file(const char* filename, int* commandCount)
{
    assert(commandCount);
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Ошибка открытия файла %s\n", filename);
        return NULL;
    }
    
    // временный массив для хранения команд
    int* tempCommands = (int*)calloc(1000, sizeof(int));
    int count = 0;
    char line[MAX_LINE_LENGTH];
    
    // заполняем ммассив данными
    while (fgets(line, sizeof(line), file))
    {
        // удаляем символ новой строки
        line[strcspn(line, "\n")] = 0;

        // преобразуем команды в числа
        if (strncmp(line, "PUSHR ", 6) == 0)
        {
            const char* reg_name = line + 6;
            Register_t reg = ParseRegisterName(reg_name);
            if (reg != (Register_t)-1)
            {
                tempCommands[count++] = OP_PUSHR;
                tempCommands[count++] = (int)reg;
            }
            else
            {
                printf("ERROR: неверный регистр %s\n", reg_name);
            }
        }

        else if (strncmp(line, "POPR ", 5) == 0)
        {
            const char* reg_name = line + 5;
            Register_t reg = ParseRegisterName(reg_name);
            if (reg != (Register_t)-1)
            {
                tempCommands[count++] = OP_POPR;
                tempCommands[count++] = (int)reg;
            }
            else
            {
                printf("ERROR: неверный регистр %s\n", reg_name);
            }
        }

        else if (strcmp(line, "EXIT") == 0) { tempCommands[count++] = OP_EXIT; }
        else if (strncmp(line, "PUSH ", 5) == 0)
        { // todo could have been done easier
            tempCommands[count++] = OP_PUSH;

            int value = 0;
            // сдвигаем указатель на 5 вправо и записываем число в value
            if (sscanf(line + 5, "%d", &value) == 1)
            {
                tempCommands[count++] = value;
            }
            else
            {
                printf("Ошибка: неверный формат числа в PUSH\n");
            }
        }
        else if (strcmp(line, "POP") == 0)   { tempCommands[count++] = OP_POP;   }
        else if (strcmp(line, "ADD") == 0)   { tempCommands[count++] = OP_ADD;   }
        else if (strcmp(line, "SUB") == 0)   { tempCommands[count++] = OP_SUB;   }
        else if (strcmp(line, "MUL") == 0)   { tempCommands[count++] = OP_MUL;   }
        else if (strcmp(line, "DIV") == 0)   { tempCommands[count++] = OP_DIV;   }
        else if (strcmp(line, "PRINT") == 0) { tempCommands[count++] = OP_PRINT; }
        else
        {
            printf("ОШИБКА: %s\n", line);
        }
    }
    
    fclose(file);
    
    // создаем финальный массив нужного размера
    int* commands = (int*)calloc(count, sizeof(int));

    memcpy(commands, tempCommands, count * sizeof(int));
    free(tempCommands);
    
    *commandCount = count;

    return commands;
}

StackErr_t execute_instruction(StackErr_t (*operation)(Stack_t*), Stack_t *stk, StackErr_t *err, const char* op_name)
{
    if (err != NULL) { *err = STACK_OK; }  // инициализация ошибки по умолчанию
    
    StackErr_t result = operation(stk);
    
    if (err != NULL) { *err = result; }
    
    if (result == STACK_OK) { printf("Команда %s выполнена успешно\n", op_name); }
    else { printf("Ошибка выполнения %s: %d\n", op_name, result); }
    
    return result;
}

// выполнение команд из массива, здесь же создание стека
void execute_commands(int* commands, int commandCount)
{
    // создаём стек
    Stack_t stk = {};
    StackErr_t err = StackInit(&stk, 15); // задаётся capacity 
    if (err != STACK_OK) {
        printf("Ошибка инициализации стека: %d\n", err);
        return;
    }
    
    int i = 0;
    while (i < commandCount)
    {
        int opcode = commands[i];
        
        switch (opcode)
        {
            case OP_PUSHR:
                if (i + 1 < commandCount)
                {
                    Register_t reg = (Register_t)commands[i + 1];
                    err = StackPushReg(&stk, reg);
                    if (err != STACK_OK)
                    {
                        printf("PUSHR ERROR: %d\n", err);
                    }
                    i++;
                }
                else
                {
                    printf("ERROR: нет регистра после PUSHR\n");
                }
                break;
                
            case OP_POPR:
                if (i + 1 < commandCount)
                {
                    Register_t reg = (Register_t)commands[i + 1];
                    err = StackPopReg(&stk, reg);
                    if (err != STACK_OK)
                    {
                        printf("POPR ERROR: %d\n", err);
                    }
                    i++;
                }
                else
                {
                    printf("ERROR: нет регистра после POPR\n");
                }
                break;

            case OP_EXIT:
                StackDestroy(&stk);
                return ;
                
            case OP_PUSH:
                if (i + 1 < commandCount)
                {
                    int value = commands[i + 1];
                    err = StackPush(&stk, value);
                    if (err == STACK_OK)
                    {
                        printf("Добавлено: %d\n", value);
                    }
                    else
                    {
                        printf("Ошибка добавления: %d\n", err);
                    }
                    i++; // двигаемся на 2 элемента
                }
                else
                {
                    printf("ERROR: нет числа после PUSH\n");
                }
                break;
                
            case OP_POP:
                {
                    StackErr_t pop_err;
                    int popped = StackPop(&stk, &pop_err);
                    if (pop_err == STACK_OK) {
                        printf("Извлечено: %d\n", popped);
                    } else {
                        printf("Ошибка извлечения: %d\n", pop_err);
                    }
                }
                break;
                
            case OP_ADD:
                execute_instruction(StackAdd, &stk, &err, "ADD");
                break;
            
            case OP_SUB:
                execute_instruction(StackSub, &stk, &err, "SUB");
                break;
                
            case OP_MUL:
                execute_instruction(StackMul, &stk, &err, "MUL");
                break;
                
            case OP_DIV:
                execute_instruction(StackDiv, &stk, &err, "DIV");
                break;
                
            case OP_PRINT:
               execute_instruction(StackPrint, &stk, &err, "PRINT");
                break;
                   
            default:
                printf("Неизвестный код операции: %d\n", opcode);
                break;
        }
        i++;
    }
    
    StackDestroy(&stk);

    return ;
}

// преобразование числового массива обратно в текст и запись в файл
void write_commands_to_file(const char* filename, int* commands, int commandCount)
{
    FILE* file = fopen(filename, "w");
    if (!file)
    {
        printf("Ошибка создания файла %s\n", filename);
        return;
    }
    
    int i = 0;
    while (i < commandCount)
    {
        int opcode = commands[i];
        
        switch (opcode)
        { 
            case OP_PUSHR:
                if (i + 1 < commandCount)
                {
                    // char reg = commands[i + 1];
                    fprintf(file, "PUSHR\n");
                    i += 2;
                }
                else
                {
                    fprintf(file, "PUSHR ERROR\n");
                    i++;
                }
                break;
                
            case OP_POPR:
                if (i + 1 < commandCount)
                {
                    // char reg = commands[i + 1];
                    fprintf(file, "POPR\n");
                    i += 2;
                }
                else
                {
                    fprintf(file, "POPR ERROR\n");
                    i++;
                }
                break;

            case OP_EXIT:
                fprintf(file, "EXIT\n");
                i++;
                break;
                
            case OP_PUSH:
                if (i + 1 < commandCount)
                {
                    int value = commands[i + 1];
                    fprintf(file, "PUSH %d\n", value);
                    i += 2;
                }
                else
                {
                    fprintf(file, "PUSH ERROR\n");
                    i++;
                }
                break;
                
            case OP_POP:
                fprintf(file, "POP\n");
                i++;
                break;
                
            case OP_ADD:
                fprintf(file, "ADD\n");
                i++;
                break;
                
            case OP_SUB:
                fprintf(file, "SUB\n");
                i++;
                break;
                
            case OP_MUL:
                fprintf(file, "MUL\n");
                i++;
                break;
                
            case OP_DIV:
                fprintf(file, "DIV\n");
                i++;
                break;
                
            case OP_PRINT:
                fprintf(file, "PRINT\n");
                i++;
                break;
                
            default:
                fprintf(file, "ERROR(%d)\n", opcode);
                i++;
                break;
        }
    }
    
    fclose(file);
    printf("Вывод в файл осуществлён");

    return ;
}


int main()
{
    const char* inputFile = "commands.txt";
    const char* outputFile = "output_commands.txt";
    
    // чтение команд из файла и преобразование в числовой массив
    int  commandCount = 0;
    int* commands = read_commands_from_file(inputFile, &commandCount);
    
    if (!commands)
    {
        printf("Не удалось прочитать команды из файла\n");
        return 1;
    }
    
    printf("Прочитано %d элементов из файла\n", commandCount);
    
    // вывод числового массива для отладки
    printf("Массив команд: ");
    for (int i = 0; i < commandCount; i++)
    {
        printf("%d ", commands[i]);
    }
    printf("\n\n");
    
    // выполнение команд
    execute_commands(commands, commandCount);

    write_commands_to_file(outputFile, commands, commandCount);
    
    // очистка памяти
    free(commands);

    return 0;
}