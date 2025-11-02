#include "structs.h"
#include "assembler.h"

int main()
{
    const char* inputFile  = "commands.txt";
    const char* outputFile = "compiled_commands.bin";
    
    // АССЕМБЛЕР
    int commandCount = 0;
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
    
    // запись скомпилированных команд в бинарный файл
    FILE* binFile = fopen(outputFile, "wb");
    if (!binFile)
    {
        printf("Ошибка создания файла %s\n", outputFile);
        free(commands);
        return 1;
    }
    
    // записываем количество команд и сам массив
    fwrite(&commandCount, sizeof(int), 1, binFile);
    fwrite(commands, sizeof(int), commandCount, binFile);
    fclose(binFile);
    
    // очистка памяти
    free(commands);

    return 0;
}