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
        ASListNode node =  malloc(sizeof(*node));
        if(node==NULL){
            return AS_OUT_OF_MEMORY;
        }
        set->first = node;
        node->value = set->copyElement(element);
        return AS_SUCCESS;
    }
    if (searchFor(set, element) != NULL) {
        return AS_ITEM_ALREADY_EXISTS;
    }

    ASListNode previous;

    AS_FOREACH(ASElement, i, set) {
            if ((set->compareElements(element, i))<0) {
                ASListNode node =  malloc(sizeof(*node));
                if(node==NULL) {
                    return AS_OUT_OF_MEMORY;
                }
                node->next = set->iterator;
                previous->next = node;
                node->value = set->copyElement(element);
                return AS_SUCCESS;
            } else {
                previous = set->iterator;
            }
    }

    ASListNode node =  malloc(sizeof(*node));
    if(node==NULL) {
        return AS_OUT_OF_MEMORY;
    }
    previous->next = node;
    node->value = set->copyElement(element);
    node->next = NULL;
    return AS_SUCCESS;
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









AmountSet asCopy(AmountSet set){
    if(set == NULL){
        return NULL;
    }
    // Create a new set
    AmountSet result =  asCreate(set->copyElement,set->freeElement,set->compareElements);
    if(result==NULL){
        return NULL;
    }
    // Insert first item
    ASListNode node = malloc(sizeof(*node));
    if(node==NULL){
        return NULL;
    }
    result->first = node;
    node->value = result->copyElement(set->first);
    // Insert next items
    AS_FOREACH(ASElement,i,set){
        ASListNode new_node = malloc(sizeof(*node));
        if(new_node==NULL){
            return NULL;
        }
        result->first = new_node;
        new_node->value = result->copyElement(i);
    }
    return result;
}



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
AmountSetResult asClear(AmountSet set) {
    if(set==NULL){
        return AS_NULL_ARGUMENT;
    }
    ASListNode ptr = set->first;
    while (ptr) {
        ASListNode remove = ptr;
        ASElement remove_value = remove->value;
        set->freeElement(remove_value); //frees ASElement
        free(remove); //frees ASListNode
        ptr = ptr->next;
    }
    return AS_SUCCESS;
}

void asDestroy(AmountSet set){
    if(set==NULL){
        return;
    }
    asClear(set);
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

AmountSetResult asGetAmount(AmountSet set, ASElement element, double *outAmount){
    if(set==NULL||element==NULL||outAmount==NULL){
        return AS_NULL_ARGUMENT;
    }
    ASListNode node = searchFor(set,element);
    if(node==NULL){
        return AS_ITEM_DOES_NOT_EXIST;
    }
    *outAmount = node->amount;
    return AS_SUCCESS;
}






