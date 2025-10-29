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
    int count = 0;
    char line[MAX_LINE_LENGTH];
    
    // ПЕРВЫЙ ПРОХОД: сбор меток и подсчёт КОМАНД (без меток)
    LabelTable labelTable;
    labelTable.count = 0;
    int current_position = 0;

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;
        
        if (line[0] == ':')
        {
            // Метка указывает на текущую позицию команды
            strncpy(labelTable.labels[labelTable.count].name, line, MAX_LINE_LENGTH - 1);
            labelTable.labels[labelTable.count].position = current_position;
            labelTable.count++;
        }
        else if (strlen(line) > 0 && !(line[0] == '/' && line[1] == '/'))
        {
            // Увеличиваем счётчик позиций команд
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
                printf("ASS ERROR: неверный регистр %s\n", reg_name);
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
                printf("ASS ERROR: неверный регистр %s\n", reg_name);
            }
        }
        else if (strncmp(line, "CALL ", 5) == 0)
        {
            tempCommands[count++] = OP_CALL;
            const char* label_name = line + 5;
            
            // ищем метку в таблице
            int label_pos = -1;
            for (int i = 0; i < labelTable.count; i++)
            {
                if (strcmp(labelTable.labels[i].name, label_name) == 0)
                {
                    label_pos = labelTable.labels[i].position - 1;
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
        else if (strcmp(line, "RET")   == 0) { tempCommands[count++] = OP_RET;    }
        else if (strcmp(line, "EXIT")  == 0) { tempCommands[count++] = OP_EXIT;   }
        else if (strcmp(line, "PUSHH") == 0) { tempCommands[count++] = OP_PUSHH;  }
        else if (strcmp(line, "POPH")  == 0) { tempCommands[count++] = OP_POPH;   }
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
        else if (strcmp(line, "POP")   == 0) { tempCommands[count++] = OP_POP;   }
        else if (strcmp(line, "ADD")   == 0) { tempCommands[count++] = OP_ADD;   }
        else if (strcmp(line, "SUB")   == 0) { tempCommands[count++] = OP_SUB;   }
        else if (strcmp(line, "MUL")   == 0) { tempCommands[count++] = OP_MUL;   }
        else if (strcmp(line, "DIV")   == 0) { tempCommands[count++] = OP_DIV;   }
        else if (strcmp(line, "SQRT")  == 0) { tempCommands[count++] = OP_SQRT;  }
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