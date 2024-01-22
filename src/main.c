#include <stdio.h>
#include "generic_management.h"
#include "NeuronsLayer.h"
#include "ActivationFunction.h"
#include "error_management.h"

int main()
{
    ActivationFunction* af;
    TRY {
        af = new_ActivationFunction2(SIGMOID, 1.0, 0.0);
    }
    CATCHALL {
        ErrorLog();
        ASSERT(0);
    };

    printf("Function activation: %.11lf", calc(af, 1.0));
    
    fgetc(stdin);

    delete_ActivationFunction(af);

    return 0;
}