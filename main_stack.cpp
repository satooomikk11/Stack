#include "structs.h"
#include "assembler.h"
#include "processor.h"
#include "disassembler.h"

// общие функции
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

const char* GetRegisterName(Register_t reg)
{
    switch (reg)
    {
        case REG_RAX: return "RAX";
        case REG_RBX: return "RBX";
        case REG_RCX: return "RCX";
        case REG_RDX: return "RDX";
        case REG_REX: return "REX";
        default: return "UNKNOWN";
    }
}

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