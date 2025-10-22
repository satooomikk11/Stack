#include "structs.h"
#include "disassembler.h"

// преобразование массива обратно в текст (деассемблер)
void write_commands_to_file(const char* filename, int* commands, int commandCount)
{
    FILE* file = fopen(filename, "w");
    if (!file)
    {
        printf("Ошибка создания файла %s\n", filename);
        return;
    }
    
    // создаем обратную таблицу меток для красивого вывода
    int labelCount = 0;
    int labelPositions[100];
    
    // сначала находим все позиции, на которые ссылаются JUMP команды
    for (int i = 0; i < commandCount; i++)
    {
        if (commands[i] == OP_JUMP && i + 1 < commandCount)
        {
            int target = commands[i + 1];
            if (target >= 0 && target < commandCount)
            {
                // проверяем, есть ли уже такая метка
                int found = 0;
                for (int j = 0; j < labelCount; j++)
                {
                    if (labelPositions[j] == target)
                    {
                        found = 1;
                        break;
                    }
                }
                if (!found && labelCount < 100)
                {
                    labelPositions[labelCount++] = target;
                }
            }
        }
    }
    
    int i = 0;
    while (i < commandCount)
    {
        // проверяем, нужно ли вывести метку
        for (int j = 0; j < labelCount; j++)
        {
            if (labelPositions[j] == i)
            {
                fprintf(file, ":label_%d\n", j);
                break;
            }
        }
        
        int opcode = commands[i];
        
        switch (opcode)
        { 
            case OP_JUMP:
                if (i + 1 < commandCount)
                {
                    int target = commands[i + 1];
                    // ищем имя метки для этого target
                    for (int j = 0; j < labelCount; j++)
                    {
                        if (labelPositions[j] == target)
                        {
                            fprintf(file, "JUMP :label_%d\n", j+1);
                            break;
                        }
                    }
                    i++;
                }
                else
                {
                    fprintf(file, "JUMP ERROR\n");
                }
                break;
                
            case OP_PUSHR:
                if (i + 1 < commandCount)
                {
                    Register_t reg = (Register_t)commands[i + 1];
                    const char* reg_name = GetRegisterName(reg);
                    fprintf(file, "PUSHR %s\n", reg_name);
                    i++;
                }
                else
                {
                    fprintf(file, "PUSHR ERROR\n");
                }
                break;
                
            case OP_POPR:
                if (i + 1 < commandCount)
                {
                    Register_t reg = (Register_t)commands[i + 1];
                    const char* reg_name = GetRegisterName(reg);
                    fprintf(file, "POPR %s\n", reg_name);
                    i++;
                }
                else
                {
                    fprintf(file, "POPR ERROR\n");
                }
                break;

            case OP_EXIT:
                fprintf(file, "EXIT\n");
                break;
                
            case OP_PUSH:
                if (i + 1 < commandCount)
                {
                    int value = commands[i + 1];
                    fprintf(file, "PUSH %d\n", value);
                    i++;
                }
                else
                {
                    fprintf(file, "PUSH ERROR\n");
                }
                break;
                
            case OP_POP:
                fprintf(file, "POP\n");
                break;
                
            case OP_ADD:
                fprintf(file, "ADD\n");
                break;
                
            case OP_SUB:
                fprintf(file, "SUB\n");
                break;
                
            case OP_MUL:
                fprintf(file, "MUL\n");
                break;
                
            case OP_DIV:
                fprintf(file, "DIV\n");
                break;
                
            case OP_PRINT:
                fprintf(file, "PRINT\n");
                break;
                
            default:
                fprintf(file, "UNKNOWN(%d)\n", opcode);
                break;
        }

        i++;
    }
    
    fclose(file);
}