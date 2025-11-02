#include "structs.h"
#include "disassembler.h"

// макрос для обработки джампов
#define WRITE_JMP_COMMAND(op_name)                          \
    case op_name:                                           \
        if (i + 1 < commandCount)                           \
        {                                                   \
            int target = commands[i + 1];                   \
            fprintf(file, #op_name " %d\n", target);        \
            i++;                                            \
        }                                                   \
        else                                                \
        {                                                   \
            fprintf(file, #op_name " ERROR\n");             \
        }                                                   \
        break;

// макрос для обработки команд без аргументов
#define WRITE_NO_ARG_COMMAND(op_name)                       \
    case op_name:                                           \
        fprintf(file, #op_name "\n");                       \
        break;

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
    
    // сначала находим все позиции, на которые ссылаются CALL команды
    for (int i = 0; i < commandCount; i++)
    {
        if (commands[i] == OP_CALL && i + 1 < commandCount)
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
                fprintf(file, ":label_%d\n", j+1);
                break;
            }
        }
        
        int opcode = commands[i];
        
        switch (opcode)
        { 
            case OP_CALL:
                if (i + 1 < commandCount)
                {
                    int target = commands[i + 1];
                    // ищем имя метки для этого target
                    for (int j = 0; j < labelCount; j++)
                    {
                        if (labelPositions[j] == target)
                        {
                            fprintf(file, "CALL :label_%d\n", j+1);
                            break;
                        }
                    }
                    i++;
                }
                else
                {
                    fprintf(file, "CALL ERROR\n");
                }
                break;
                
            WRITE_JMP_COMMAND(OP_JMP)
            WRITE_JMP_COMMAND(OP_JE )
            WRITE_JMP_COMMAND(OP_JNE)
            WRITE_JMP_COMMAND(OP_JG )
            WRITE_JMP_COMMAND(OP_JL )
            WRITE_JMP_COMMAND(OP_JGE)
            WRITE_JMP_COMMAND(OP_JLE)
                
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
                
            WRITE_NO_ARG_COMMAND(OP_PUSHH)
            WRITE_NO_ARG_COMMAND(OP_POPH)
            WRITE_NO_ARG_COMMAND(OP_RET)
            WRITE_NO_ARG_COMMAND(OP_EXIT)
            WRITE_NO_ARG_COMMAND(OP_POP)
            WRITE_NO_ARG_COMMAND(OP_ADD)
            WRITE_NO_ARG_COMMAND(OP_SUB)
            WRITE_NO_ARG_COMMAND(OP_MUL)
            WRITE_NO_ARG_COMMAND(OP_DIV)
            WRITE_NO_ARG_COMMAND(OP_SQRT)
            WRITE_NO_ARG_COMMAND(OP_PRINT)
                
            default:
                fprintf(file, "UNKNOWN(%d)\n", opcode);
                break;
        }

        i++;
    }
    
    fclose(file);
}