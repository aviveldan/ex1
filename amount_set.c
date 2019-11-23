//
// Created by avive on 15/11/2019.
//
#define NULL_POINTER_SENT (-1)
#include "amount_set.h"
#include <stdlib.h>
#define myMalloc(type,pointer,return_value) \
type pointer =  malloc(sizeof((*(pointer)))); \
if((pointer)==NULL){ \
return (return_value); \
}



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
static ASListNode createNode(AmountSet set,ASElement value,ASListNode next){
    if(set==NULL||value==NULL){
        return false;
    }
    myMalloc(ASListNode,result,NULL);
    result->value=set->copyElement(value);
    if(result->value == NULL){//if copyElement failed to allocate memory
        set->freeElement(result->value);
        free(result);
        return NULL;
    }
    result->amount = 0;
    result->next=next;
    return result;
}

//Searches an element in the list and returns a pointer to it.
//will return NULL if element isn't in the list.
ASListNode searchFor(AmountSet set, ASElement element){
    if((set==NULL)||(set->first==NULL)){
        return NULL;
    }
    
    if(set->compareElements(set->first->value,element)==0) {
        return set->first;
    }
    ASListNode i = set->first;
    for(;(i->next)!=NULL;(i=i->next)){
        if(set->compareElements(i->value,element)==0){
            set->iterator = i;
            return i;
        }
    }
    if(i->next==NULL && (set->compareElements(i->value,element)==0)) {
        return i;
    }
    return NULL;
}

//adds a new element to the list if element isn't in the list.
//keeps list sorted
AmountSetResult asRegister(AmountSet set, ASElement element) {
    if (set==NULL||element==NULL) {
        return AS_NULL_ARGUMENT;
    }
    //if no elements in the list
    if (set->first==NULL) {
        set->first=createNode(set,element,NULL);
        if(set->first==NULL){
            return AS_OUT_OF_MEMORY;
        }
        set->iterator=set->first;
        (set->size)++;
        return AS_SUCCESS;
    }
    //if element value is less then first element
    if(set->compareElements(element,set->first->value)<0){
        ASListNode node = createNode(set,element,set->first);
        if(node==NULL){
            return AS_OUT_OF_MEMORY;
        }
        set->first = node;
        (set->size)++;
        set->iterator=set->first;
        return AS_SUCCESS;
    }
    //if element is in the list
    if (asContains(set,element)) {
        return AS_ITEM_ALREADY_EXISTS;
    }
    //now element value is greater than the first in the list, and the list isn't empty:
    set->iterator = set->first;
    ASListNode previous = set->iterator;
    //find where the element belongs:
    //in example: element is *5* and list is 1,4,7
    //list will now be 1,4,*5*,7
    for(ASListNode current=set->iterator;current!=NULL;current=current->next) {
        if ((set->compareElements(element, current->value)) < 0) {
            ASListNode node = createNode(set,element,set->iterator);
            if(node==NULL){
                return AS_OUT_OF_MEMORY;
            }
            previous->next = node;
            (set->size)++;
            return AS_SUCCESS;

        } else
            previous = set->iterator;
        set->iterator = set->iterator->next;
    }
    //element is greater than all existing element in list- so place it in the end
    //previous now will point to the end of the list
    ASListNode node = createNode(set,element,NULL);
    if(node==NULL){
        return AS_OUT_OF_MEMORY;
    }
    (set->size)++;
    previous->next = node;
    return AS_SUCCESS;
}

//assigns iterator to the first element of the set and returns the value of the first element.
ASElement asGetFirst(AmountSet set) {
    if (set==NULL || set->first==NULL ||(set->first==NULL && set->iterator==NULL)) {
        return NULL;
    } else {
        set->iterator=set->first;
        //return set->copyElement(set->first->value);
        return set->first->value;
    }
}

AmountSet asCreate(CopyASElement copyElement,
                   FreeASElement freeElement,
                   CompareASElements compareElements){
    if(copyElement==NULL || freeElement == NULL || compareElements == NULL){
        return NULL;
    }
    myMalloc(AmountSet,result,NULL);
    result->copyElement=copyElement;
    result->freeElement=freeElement;
    result->compareElements=compareElements;

    result->first = NULL;
    result->iterator = NULL;
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
    result->size = set->size;
    // Insert first item
    if(set->first == NULL){
        result->first = NULL;
        return result;
    }
    ASListNode iterator = set->first;
    while(iterator!=NULL){
        asRegister(result,iterator->value);
        iterator=iterator->next;
    }
    return result;
}

ASElement asGetNext(AmountSet set){
    if(set==NULL){
        return NULL;
    }
    if(set->iterator == NULL) {
        return NULL;
    }
    if(set->iterator->next == NULL) {
        return NULL;
    }
    set->iterator = set->iterator->next;
    //return set->copyElement(set->iterator->value);
    return set->iterator->value;
}

//clearing the list as deleting first element over and over
AmountSetResult asClear(AmountSet set) {
    if(set==NULL){
        return AS_NULL_ARGUMENT;
    }
    if(set->first == NULL){
        return AS_SUCCESS;
    }
    while(set->first != NULL){
        asDelete(set,set->first->value);
    }
    set->size = 0;
    return AS_SUCCESS;
}


void asDestroy(AmountSet set){
    if(set==NULL){
        return;
    }
    if((set->first)!=NULL) {
        asClear(set); //destroys the list without removing function
    }
    free(set);//frees AmountSet
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
    if(set->first==NULL){
        return AS_ITEM_DOES_NOT_EXIST;
    }
    ASListNode node = searchFor(set,element);
    if(node==NULL){
        return AS_ITEM_DOES_NOT_EXIST;
    }
    if(((node->amount)+amount)<0){
        return AS_INSUFFICIENT_AMOUNT;
    }
    (node->amount) += amount;
    return AS_SUCCESS;
}

AmountSetResult asDelete(AmountSet set, ASElement element){
    if(set==NULL || element == NULL){
        return AS_NULL_ARGUMENT;
    }
    ASListNode node = searchFor(set,element);
    if(set->first == NULL || node==NULL){
        return AS_ITEM_DOES_NOT_EXIST;
    }
    if(set->first == node && node->next == NULL ){
        set->freeElement(node->value);
        free(node);
        set->first = NULL;
        set->iterator =NULL;
        (set->size)--;
        return AS_SUCCESS;
    }
    ASListNode beg = set->first;

    if(beg==node){//if wants to delete the first element but there is a next node
        set->first = set->first->next; //next node is now the first
        set->freeElement(beg->value); //free prev node
        free(beg);
        (set->size)--;
        return AS_SUCCESS;
    }
    //save the value before
    while(beg->next != NULL){
        if(beg->next == node){
            beg->next = node->next; //A->B->C will be changed to A->C
            set->freeElement(node->value);
            free(node);
            (set->size)--;
            return AS_SUCCESS;
        }
        beg=beg->next;
    }

    return AS_SUCCESS;
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






