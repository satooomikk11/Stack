#include "structs.h"
#include "assembler.h"

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
    
    // временный массив для хранения команд
    int* tempCommands = (int*)calloc(1000, sizeof(int));
    int  count = 0;
    char line[MAX_LINE_LENGTH];
    
    // первый проход: сбор меток
    LabelTable labelTable;
    labelTable.count = 0;
    
    int lineNumber = 0;
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;
        
        // проверяем, является ли строка меткой (начинается с :)
        if (line[0] == ':')
        {
            // сохраняем метку и её позицию в массиве команд
            strncpy(labelTable.labels[labelTable.count].name, line, MAX_LINE_LENGTH - 1);
            labelTable.labels[labelTable.count].position = count;
            labelTable.count++;
        }
        else if (strlen(line) > 0 && !(line[0] == '/' && line[1] == '/'))
        {
            // подсчитываем только команды (не метки и не комментарии)
            if (strncmp(line, "PUSHR ", 6) == 0 || 
                strncmp(line, "POPR " , 5) == 0 ||
                strncmp(line, "JUMP " , 5) == 0 ||
                strncmp(line, "PUSH " , 5) == 0)
            {
                count += 2; // команда + аргумент
            }
            else
            {
                count++; // команда без аргумента
            }
        }
        lineNumber++;
    }
    
    // второй проход: преобразование команд
    rewind(file);
    count = 0;
    
    while (fgets(line, sizeof(line), file))
    {
        // удаляем символ новой строки
        line[strcspn(line, "\n")] = 0;

        // пропускаем метки
        if (line[0] == ':' || strlen(line) == 0)
            continue;

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
        else if (strncmp(line, "JUMP ", 5) == 0)
        {
            tempCommands[count++] = OP_JUMP;
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
                tempCommands[count++] = label_pos;
            }
            else
            {
                printf("Ошибка: метка %s не найдена\n", label_name);
                tempCommands[count++] = -1;
            }
        }
        else if (strcmp(line, "EXIT") == 0) { tempCommands[count++] = OP_EXIT; }
        else if (strncmp(line, "PUSH ", 5) == 0)
        {
            tempCommands[count++] = OP_PUSH;
            int value = 0;
            if (sscanf(line + 5, "%d", &value) == 1)
            {
                tempCommands[count++] = value;
            }
            else
            {
                printf("Ошибка: неверный формат числа в PUSH\n");
            }
        }
        else if (strcmp(line, "POP"  ) == 0) { tempCommands[count++] = OP_POP;   }
        else if (strcmp(line, "ADD"  ) == 0) { tempCommands[count++] = OP_ADD;   }
        else if (strcmp(line, "SUB"  ) == 0) { tempCommands[count++] = OP_SUB;   }
        else if (strcmp(line, "MUL"  ) == 0) { tempCommands[count++] = OP_MUL;   }
        else if (strcmp(line, "DIV"  ) == 0) { tempCommands[count++] = OP_DIV;   }
        else if (strcmp(line, "PRINT") == 0) { tempCommands[count++] = OP_PRINT; }
        else
        {
            printf("ОШИБКА: неизвестная команда %s\n", line);
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