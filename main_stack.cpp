#include "structs.h"
#include "assembler.h"
#include "processor.h"
#include "disassembler.h"

int main()
{
    const char* inputFile  = "commands.txt";
    const char* outputFile = "output_commands.txt";
    
    // АССЕМБЛЕР
    int  commandCount = 0;
    int* commands = read_commands_from_file(inputFile, &commandCount);
    
    if (!commands)
    {
        printf("Не удалось прочитать команды из файла\n");
        return 1;
    }
    
    printf("Прочитано %d элементов из файла\n", commandCount);
    
    // вывод массива для отладки
    printf("Массив команд: ");
    for (int i = 0; i < commandCount; i++)
    {
        printf("%d ", commands[i]);
    }
    printf("\n\n");
    
    // ПРОЦЕССОР
    StackErr_t result = execute_commands(commands, commandCount);
    if (result != STACK_OK)
    {
        printf("Ошибка выполнения команд: %d\n", result);
    }

    // ДИСАССЕМБЛЕР
    write_commands_to_file(outputFile, commands, commandCount);
    
    // очистка памяти
    free(commands);

    return 0;
}