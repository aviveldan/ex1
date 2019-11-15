//
// Created by avive on 15/11/2019.
//
#include "amount_set.h"
#include <stdlib.h>
struct AmountSet_t{
    ASElement first;
    CopyASElement copyElement;
    FreeASElement freeElement;
    CompareASElements compareElements;
};

AmountSet asCreate(CopyASElement copyElement,
                   FreeASElement freeElement,
                   CompareASElements compareElements){
    if(!(copyElement&&freeElement&&compareElements)){
        return AS_NULL_ARGUMENT;
    }
    AmountSet result = NULL;
    result = malloc(sizeof(*result));
    if(result==NULL){
        return AS_OUT_OF_MEMORY;
    }
    result->copyElement=copyElement;
    result->freeElement=freeElement;
    result->compareElements=compareElements;
    result->first = NULL;
    return result;
}



