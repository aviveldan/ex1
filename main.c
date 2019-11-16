#include <stdlib.h>
#include <stdio.h>
#include "amount_set.h"

ASElement copyInt(ASElement element){
    if(element==NULL){
        return NULL;
    }
    int* newInt = malloc(sizeof(int));
    if(newInt == NULL){
        return NULL;
    }
    *newInt = *(int*)element;
    return newInt;
}
void freeInt(ASElement element){
    free(element);
}
int compareInt(ASElement element1,ASElement element2){
    int ele1 = *(int*)element1;
    int ele2 = *(int*)element2;
    int result = 0;
    if(ele1>ele2){
        result = 1;
    }
    else if(ele1<ele2){
        result = -1;
    }
    else
        result = 0;
    return result;
}

int main() {
    AmountSet aviv = asCreate(copyInt,freeInt,compareInt);
    asGetSize(aviv);
    /*
    int i = 8;
    asRegister(aviv,&i);
    int* l = asGetFirst(aviv);
    printf("hello %d",*l);
    */
    AmountSet lior = asCopy(aviv);
    asDestroy(aviv);
    asDestroy(lior);
    return 0;
}