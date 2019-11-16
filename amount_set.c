//
// Created by avive on 15/11/2019.
//
#include "amount_set.h"
#include <stdlib.h>

typedef struct ASListNode_t {
    struct set* next;
    ASElement value;
    double amount;
} *ASListNode;

struct AmountSet_t{
    ASListNode first;
    CopyASElement copyElement;
    FreeASElement freeElement;
    CompareASElements compareElements;
    int iterator;
};



AmountSet asCreate(CopyASElement copyElement,
                   FreeASElement freeElement,
                   CompareASElements compareElements){
    if(!(copyElement&&freeElement&&compareElements)){
        return NULL;
    }
    AmountSet result = malloc(sizeof(*result)); //Allocating new set
    if(result==NULL){
        return NULL;
    }

    result->copyElement=copyElement;
    result->freeElement=freeElement;
    result->compareElements=compareElements;
    result->first = NULL;
    result->iterator = 0;
    return result;
}

/* //Waiting for function for copying Lists
AmountSet asCopy(AmountSet set){
    if(set == NULL){
        return NULL;
    }
    AmountSet result =  asCreate(set->copyElement,set->freeElement,set->copyElement);
}
*/


ASElement asGetNext(AmountSet set){
    (set->iterator)++;
}


void asDestroy(AmountSet set){
    if(set==NULL){
        return;
    }
    ASListNode ptr = set->first;
    while(ptr){
        ASListNode remove = ptr;
        ASElement remove_value = remove->value;
        set->freeElement(remove_value); //frees ASElement
        free(remove); //frees ASListNode
        ptr=ptr->next;
    }
    free(set); //frees AmountSet
}




