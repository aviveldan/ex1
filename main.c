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
static void addElements(AmountSet set) {
    int ids[7] = {0, 4, 2, 5, 7, 3, 1};
    for (int i = 0; i < 7; ++i) {
        asRegister(set, ids + i);
    }
    asChangeAmount(set, ids+6 , 1);
    asChangeAmount(set, ids + 2, 2);
    asChangeAmount(set, ids + 5, 3);
    asChangeAmount(set, ids , 1000);

}
int main3() {
    AmountSet aviv = asCreate(copyInt,freeInt,compareInts);
    addElements(aviv);
    int k = -2;
    asRegister(aviv,&k);
    asChangeAmount(aviv, &k , 1500);
    for(int i = 0;i<9;i++){
        if(asGetFirst(aviv)==NULL){
            break;
        }
        printf("deleting %d \n",*(int*)asGetFirst(aviv));
        if(i==6){
            int z = 7;
            asChangeAmount(aviv, &z , 999);
        }
        asDelete(aviv,asGetFirst(aviv));
    }


    addElements(aviv);
    int j = -2;
    asRegister(aviv,&j);
    asChangeAmount(aviv, &j , 1500);

    asClear(aviv);


    asDestroy(aviv);
    //asGetSize(aviv);
    /*
    int i = 8;
    int j = 9;
    asRegister(aviv,&i);
    asRegister(aviv,&j);
    AS_FOREACH(ASElement,i,aviv){
        printf("this is %d\n",*(int*)i);
    }

    if(asClear(aviv)==AS_SUCCESS){
        printf("Hello");
        asRegister(aviv,&i);
        asRegister(aviv,&j);
        AS_FOREACH(ASElement,i,aviv){
            printf("this is %d\n",*(int*)i);
        }
    }
    else
        printf("Demage!!");

    asDestroy(aviv);
    aviv = NULL;
    printf("arrived here");
     */
    return 0;
}