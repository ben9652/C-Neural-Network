#include <stdio.h>
#include "vendor/vc_vector/vc_vector_tests.h"

int main() {
    vc_vector_run_tests();
    printf("Tests passed.\n");
    fgetc(stdin);
    return 0;
}