//
// Created by avive on 15/11/2019.
//
#define NULL_POINTER_SENT -1
#include "amount_set.h"
#include <stdlib.h>

typedef struct ASListNode_t {
    struct ASListNode_t* next;
    ASElement value;
    double amount;
} *ASListNode;

struct AmountSet_t{
    ASListNode first;
    CopyASElement copyElement;
    FreeASElement freeElement;
    CompareASElements compareElements;
    ASListNode iterator;
    int size;
};

ASListNode searchFor(AmountSet set, ASElement element){
    AS_FOREACH(ASElement,i,set){
        if(set->compareElements(i,element) == 0){
            return set->iterator;
        }
        else
            return false;
    }
}

AmountSetResult asRegister(AmountSet set, ASElement element) {
    if (element==NULL) {
        return AS_NULL_ARGUMENT;
    }
    if (set->first==NULL) {
        set->first=set->copyElement(element);
        return AS_SUCCESS;
    }
    if (searchFor(set, element) != NULL) {
        return AS_ITEM_ALREADY_EXISTS;
    } else {
        AS_FOREACH(ASElement, i, set) {
            if (set->compareElements(element, i))
        }
    }
}

//assigns iterator to the first element of the set and returns the value of the first element.
ASElement asGetFirst(AmountSet set) {
    if (set==NULL || set->first==NULL) {
        return NULL;
    } else {
        set->iterator=set->first;
        return set->first->value;
    }
}


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
    result->size = 0;
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
    if(set==NULL){
        return NULL;
    }

    set->iterator = set->iterator->next;
    if(set->iterator == NULL) {
        return NULL;
    }
    return set->iterator->value;
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

int asGetSize(AmountSet set){
    if(set==NULL){
        return NULL_POINTER_SENT;
    }
    return set->size;
}
bool asContains(AmountSet set, ASElement element){
    if(searchFor(set,element)==NULL){
        return false;
    }
    else
        return true;
}

AmountSetResult asChangeAmount(AmountSet set, ASElement element, const double amount){
    if(set==NULL || element == NULL){
        return AS_NULL_ARGUMENT;
    }
    ASListNode node = searchFor(set,element);
    if(node==NULL){
        return AS_ITEM_DOES_NOT_EXIST;
    }
    if(((node->amount)+amount)<0){
        return AS_INSUFFICIENT_AMOUNT;
    }
    node->amount += amount;
    return AS_SUCCESS;
}

AmountSetResult asDelete(AmountSet set, ASElement element){
    if(set==NULL || element == NULL){
        return AS_NULL_ARGUMENT;
    }
    ASListNode node = searchFor(set,element);
    if(node==NULL){
        return AS_ITEM_DOES_NOT_EXIST;
    }
    ASListNode item = set->first;
    for(; item->next==NULL ; item=item->next){ \
        if(item->next==node){
            item->next = node->next;
            set->freeElement(node->value);
            free(node);
        }
    }
}






