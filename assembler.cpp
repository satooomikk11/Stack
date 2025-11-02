#include "structs.h"
#include "assembler.h"

Register_t ParseRegisterName(const char* reg_name)
{
    assert(reg_name);

    if (strcmp(reg_name, "RAX") == 0) return REG_RAX;
    if (strcmp(reg_name, "RBX") == 0) return REG_RBX;
    if (strcmp(reg_name, "RCX") == 0) return REG_RCX;
    if (strcmp(reg_name, "RDX") == 0) return REG_RDX;
    if (strcmp(reg_name, "REX") == 0) return REG_REX;
    return (Register_t)-1;
}

// чтение из файла и преобразование в массив (ассемблер)
int* read_commands_from_file(const char* filename, int* commandCount)
{
    assert(commandCount);
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("Ошибка открытия файла %s\n", filename);
        return NULL;
    }
    
    // динамический массив для хранения команд
    int* commands = (int*)calloc(INITIAL_COMMAND_CAPACITY, sizeof(int));
    if (!commands)
    {
        fclose(file);
        return NULL;
    }
    
    int capacity = INITIAL_COMMAND_CAPACITY;
    int count = 0;
    char line[MAX_LINE_LENGTH];
    
    // ПЕРВЫЙ ПРОХОД: сбор меток и подсчёт КОМАНД (без меток)
    LabelTable labelTable;
    LabelTable_init(&labelTable);
    int current_position = 0;

    while (fgets(line, MAX_LINE_LENGTH, file))
    {
        line[strcspn(line, "\n")] = 0;
        
        if (line[0] == ':')
        {
            // метка указывает на текущую позицию команды
            LabelTable_add(&labelTable, line, current_position);
        }
        else if (strlen(line) > 0 && !(line[0] == '/' && line[1] == '/'))
        {
            // увеличиваем счётчик позиций команд
            if (strncmp(line, "PUSHR ", 6) == 0 || 
                strncmp(line, "POPR " , 5) == 0 ||
                strncmp(line, "CALL " , 5) == 0 ||
                strncmp(line, "PUSH " , 5) == 0)
            {
                current_position += 2; // команда + аргумент
            }
            else
            {
                current_position += 1; // команда без аргумента
            }
        }
    }

    // ВТОРОЙ ПРОХОД: преобразование команд
    rewind(file);
    count = 0;
    
    while (fgets(line, sizeof(line), file))
    {
        // удаляем символ новой строки
        line[strcspn(line, "\n")] = 0;

        // пропускаем метки и пустые строки
        if (line[0] == ':' || strlen(line) == 0)
            continue;

        // проверяем, нужно ли расширить массив команд
        if (count + 2 >= capacity)
        {
            int new_capacity = capacity * 2;
            int* new_commands = (int*)realloc(commands, new_capacity * sizeof(int));
            if (!new_commands)
            {
                printf("Ошибка: не удалось расширить массив команд\n");
                free(commands);
                LabelTable_destroy(&labelTable);
                fclose(file);
                return NULL;
            }
            commands = new_commands;
            capacity = new_capacity;
        }

        // преобразуем команды в числа
        if (strncmp(line, "JMP ", 4) == 0)
        {
            commands[count++] = OP_JMP;
            int target = 0;
            if (sscanf(line + 4, "%d", &target) == 1)
            {
                commands[count++] = target;
            }
            else
            {
                printf("Ошибка: неверный формат адреса в JMP\n");
            }
        }
        else if (strncmp(line, "JE ", 3) == 0)
        {
            commands[count++] = OP_JE;
            int target = 0;
            if (sscanf(line + 3, "%d", &target) == 1)
            {
                commands[count++] = target;
            }
            else
            {
                printf("Ошибка: неверный формат адреса в JE\n");
            }
        }
        else if (strncmp(line, "JNE ", 4) == 0)
        {
            commands[count++] = OP_JNE;
            int target = 0;
            if (sscanf(line + 4, "%d", &target) == 1)
            {
                commands[count++] = target;
            }
            else
            {
                printf("Ошибка: неверный формат адреса в JNE\n");
            }
        }
        else if (strncmp(line, "JG ", 3) == 0)
        {
            commands[count++] = OP_JG;
            int target = 0;
            if (sscanf(line + 3, "%d", &target) == 1)
            {
                commands[count++] = target;
            }
            else
            {
                printf("Ошибка: неверный формат адреса в JG\n");
            }
        }
        else if (strncmp(line, "JL ", 3) == 0)
        {
            commands[count++] = OP_JL;
            int target = 0;
            if (sscanf(line + 3, "%d", &target) == 1)
            {
                commands[count++] = target;
            }
            else
            {
                printf("Ошибка: неверный формат адреса в JL\n");
            }
        }
        else if (strncmp(line, "JGE ", 4) == 0)
        {
            commands[count++] = OP_JGE;
            int target = 0;
            if (sscanf(line + 4, "%d", &target) == 1)
            {
                commands[count++] = target;
            }
            else
            {
                printf("Ошибка: неверный формат адреса в JGE\n");
            }
        }
        else if (strncmp(line, "JLE ", 4) == 0)
        {
            commands[count++] = OP_JLE;
            int target = 0;
            if (sscanf(line + 4, "%d", &target) == 1)
            {
                commands[count++] = target;
            }
            else
            {
                printf("Ошибка: неверный формат адреса в JLE\n");
            }
        }
        else if (strncmp(line, "PUSHR ", 6) == 0)
        {
            const char* reg_name = line + 6;
            Register_t reg = ParseRegisterName(reg_name);
            if (reg != (Register_t)-1)
            {
                commands[count++] = OP_PUSHR;
                commands[count++] = (int)reg;
            }
            else
            {
                printf("ASS ERROR: неверный регистр %s\n", reg_name);
            }
        }
        else if (strncmp(line, "POPR ", 5) == 0)
        {
            const char* reg_name = line + 5;
            Register_t reg = ParseRegisterName(reg_name);
            if (reg != (Register_t)-1)
            {
                commands[count++] = OP_POPR;
                commands[count++] = (int)reg;
            }
            else
            {
                printf("ASS ERROR: неверный регистр %s\n", reg_name);
            }
        }
        else if (strncmp(line, "CALL ", 5) == 0)
        {
            commands[count++] = OP_CALL;
            const char* label_name = line + 5;
            
            // ищем метку в таблице
            int label_pos = -1;
            for (int i = 0; i < labelTable.count; i++)
            {
                if (strcmp(labelTable.labels[i].name, label_name) == 0)
                {
                    label_pos = labelTable.labels[i].position;
                    break;
                }
            }
            
            if (label_pos != -1)
            {
                commands[count++] = label_pos;
            }
            else
            {
                printf("Ошибка: метка %s не найдена\n", label_name);
                commands[count++] = -1;
            }
        }
        else if (strcmp(line, "RET")   == 0) { commands[count++] = OP_RET;    }
        else if (strcmp(line, "EXIT")  == 0) { commands[count++] = OP_EXIT;   }
        else if (strcmp(line, "PUSHH") == 0) { commands[count++] = OP_PUSHH;  }
        else if (strcmp(line, "POPH")  == 0) { commands[count++] = OP_POPH;   }
        else if (strncmp(line, "PUSH ", 5) == 0)
        {
            commands[count++] = OP_PUSH;
            int value = 0;
            if (sscanf(line + 5, "%d", &value) == 1)
            {
                commands[count++] = value;
            }
            else
            {
                printf("Ошибка: неверный формат числа в PUSH\n");
            }
        }
        else if (strcmp(line, "POP")   == 0) { commands[count++] = OP_POP;   }
        else if (strcmp(line, "ADD")   == 0) { commands[count++] = OP_ADD;   }
        else if (strcmp(line, "SUB")   == 0) { commands[count++] = OP_SUB;   }
        else if (strcmp(line, "MUL")   == 0) { commands[count++] = OP_MUL;   }
        else if (strcmp(line, "DIV")   == 0) { commands[count++] = OP_DIV;   }
        else if (strcmp(line, "SQRT")  == 0) { commands[count++] = OP_SQRT;  }
        else if (strcmp(line, "PRINT") == 0) { commands[count++] = OP_PRINT; }
        else
        {
            printf("ОШИБКА: неизвестная команда %s\n", line);
        }
    }
    
    fclose(file);
    LabelTable_destroy(&labelTable);
    
    *commandCount = count;    
    return commands;
}