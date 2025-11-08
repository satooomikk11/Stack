#include "structs.h"
#include "processor.h"

/*
// ПРОЦЕССОР
StackErr_t result = execute_commands(commands, commandCount);
if (result != STACK_OK)
{
    printf("Ошибка выполнения команд: %d\n", result);
}
*/

int main()
{
    const char* inputFile = "compiled_commands.bin";
    
    // чтение скомпилированных команд из бинарного файла
    FILE* binFile = fopen(inputFile, "rb");
    if (!binFile)
    {
        printf("Не удалось открыть файл %s\n", inputFile);
        return 1;
    }
    
    int commandCount = 0;
    // читаем кол-во команд
    fread(&commandCount, sizeof(int), 1, binFile);
    
    int* commands = (int*)calloc(commandCount, sizeof(int));
    if (!commands)
    {
        printf("Ошибка выделения памяти\n");
        fclose(binFile);
        return 1;
    }
    
    fread(commands, sizeof(int), commandCount, binFile);
    fclose(binFile);
    
    printf("Процессор: загружено %d команд\n", commandCount);
    
    // ПРОЦЕССОР
    StackErr_t result = execute_commands(commands, commandCount);
    if (result != STACK_OK)
    {
        printf("Ошибка выполнения команд: %d\n", result);
    }

    // очистка памяти
    free(commands);

    return 0;
}