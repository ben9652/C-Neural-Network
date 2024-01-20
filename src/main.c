#include <stdio.h>
#include "generic_management.h"

int main()
{
    float number;
    printf("Datatype of number: %s", typename(number));
    
    fgetc(stdin);
    return 0;
}