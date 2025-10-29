#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "structs.h"

StackErr_t execute_commands(int* commands, int commandCount);
StackErr_t processor_init  (Processor* proc, unsigned stack_capacity);

StackErr_t StackAdd  (Stack_t *stk);
StackErr_t StackSub  (Stack_t *stk);
StackErr_t StackMul  (Stack_t *stk);
StackErr_t StackDiv  (Stack_t *stk);
StackErr_t StackSqrt (Stack_t *stk);
StackErr_t StackPrint(Stack_t *stk);
StackErr_t StackPushH(Stack_t *stk, RAM* ram);
StackErr_t StackPopH (Stack_t *stk, RAM* ram);

#endif // PROCESSOR_H