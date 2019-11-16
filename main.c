#include <stdlib.h>
#include <stdio.h>
#include "amount_set.h"

static ASElement copyInt(ASElement number) {
int *copy = malloc(sizeof(*copy));
if (copy != NULL) {
*copy = *(int *)number;
}
return copy;
}

static void freeInt(ASElement number) {
    free(number);
}

static int compareInts(ASElement lhs, ASElement rhs) {
    return (*(int *)lhs) - (*(int *)rhs);
}

int main() {
    AmountSet aviv = asCreate(copyInt,freeInt,compareInts);
    asGetSize(aviv);

    int i = 8;
    int* ptr = &i;
    asRegister(aviv,ptr);

    int* l = asGetFirst(aviv);

    printf("hello %d",*l);
    asClear(aviv);
    asDestroy(aviv);
    return 0;
}