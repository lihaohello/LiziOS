#include "../include/assert.h"
#include "../include/stdio.h"
#include "../include/interrupt.h"

void panic_spin(char *filename,
                int line,
                const char *func,
                const char *condition)
{
    intr_disable();

    printf("\n-----ASSERT-----\n");
    printf("filename: %s\n", filename);
    printf("line: %d\n", line);
    printf("function: %s\n", func);
    printf("condition: %s\n", condition);
    printf("----------------\n");

    while (1)
    {
    };
}