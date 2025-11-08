#include "structs.h"
#include "disassembler.h"

int main()
{
    const char* inputFile = "compiled_commands.bin";
    const char* outputFile = "output_commands.txt";
    
    // чтение скомпилированных команд из бинарного файла
    FILE* binFile = fopen(inputFile, "rb");
    if (!binFile)
    {
        printf("Не удалось открыть файл %s\n", inputFile);
        return 1;
    }
    
    int commandCount = 0;
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
    
    // ДИСАССЕМБЛЕР
    write_commands_to_file(outputFile, commands, commandCount);
    
    // очистка памяти
    free(commands);

    return 0;
}