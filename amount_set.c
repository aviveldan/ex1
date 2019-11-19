//
// Created by avive on 15/11/2019.
//
#define NULL_POINTER_SENT -1
#include "amount_set.h"
#include <stdlib.h>
#define myMalloc(type,pointer,return_value) \
type pointer =  malloc(sizeof((*pointer))); \
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

ASListNode searchFor(AmountSet set, ASElement element){
    if((set==NULL)||(set->first==NULL)){
        return NULL;
    }
    
    if(set->compareElements(set->first->value,element)==0) {
        return set->first;
    }
    ASListNode i = set->first;
    for(;(i->next)!=NULL;(i=i->next)){
        //printf("COMPARING number %d to element: %d \n",*(int*)(i->value),*(int*)element);
        if(set->compareElements(i->value,element)==0){
            //set->iterator = i;
            return i;
        }
    }
    if(i->next==NULL && (set->compareElements(i->value,element)==0)) {
        return i;
    }
    return NULL;
}

AmountSetResult asRegister(AmountSet set, ASElement element) {
    if (element==NULL) {
        return AS_NULL_ARGUMENT;
    }
    if (set->first==NULL) {
        myMalloc(ASListNode,node,AS_OUT_OF_MEMORY);
        set->first = node;
        node->amount = 0;
        node->value = set->copyElement(element);
        node->next=NULL;
        (set->size)++;
        return AS_SUCCESS;
    }
    if(set->compareElements(element,set->first->value)<0){
        myMalloc(ASListNode,node,AS_OUT_OF_MEMORY);
        node->value = set->copyElement(element);
        ASListNode temp = set->first;
        set->first = node;
        node->next = temp;
        node->amount = 0;
        return AS_SUCCESS;
    }
    if (asContains(set,element)) {
        return AS_ITEM_ALREADY_EXISTS;
    }
    set->iterator = set->first;

    ASListNode previous = set->iterator;
    for(ASListNode current=set->iterator;current!=NULL;current=current->next) {
        if ((set->compareElements(element, current->value)) < 0) {
            myMalloc(ASListNode,node,AS_OUT_OF_MEMORY);
            node->next = set->iterator;
            previous->next = node;
            node->value = set->copyElement(element);
            node->amount = 0;
            (set->size)++;
            return AS_SUCCESS;

        } else
            previous = set->iterator;
        set->iterator = set->iterator->next;
    }
    myMalloc(ASListNode,node,AS_OUT_OF_MEMORY);
    (set->size)++;
    previous->next = node;
    node->value = set->copyElement(element);
    node->next = NULL;
    return AS_SUCCESS;
}

//assigns iterator to the first element of the set and returns the value of the first element.
ASElement asGetFirst(AmountSet set) {
    if (set==NULL || set->first==NULL || set->iterator==NULL) {
        return NULL;
    } else {
        set->iterator=set->first;
        return set->copyElement(set->first->value);
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


static void copyNodeData(AmountSet set,ASListNode target, ASListNode source){
    if(target==NULL || source==NULL || set==NULL){
        return;
    }
    target->amount=source->amount;
    target->value = set->copyElement(source->value);
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
    myMalloc(ASListNode,newNode,NULL);
    copyNodeData(set,newNode,set->first);
    result->first = newNode;
    // Insert next items
    ASListNode set_next_item = set->first->next;
    while(set_next_item!=NULL){
        myMalloc(ASListNode,next_newNode,NULL);
        newNode->next = next_newNode;
        copyNodeData(set,next_newNode,set_next_item);
        set_next_item=set_next_item->next;
        newNode=newNode->next;
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
    set->iterator = set->iterator->next;
    if(set->iterator == NULL) {
        return NULL;
    }
    return set->copyElement(set->iterator->value);
}

AmountSetResult asClear(AmountSet set) {
    if(set==NULL){
        return AS_NULL_ARGUMENT;
    }
    if(set->first == NULL){
        return AS_SUCCESS;
    }
    while(set->first != NULL){
        asDelete(set,asGetFirst(set));
    }
    set->size = 0;
    return AS_SUCCESS;
}

void asDestroy(AmountSet set){
    if(set==NULL){
        return;
    }
    if((set->first)!=NULL) {
        asClear(set); //destroys the list without removing functions
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
        return AS_SUCCESS;
    }
    ASListNode beg = set->first;

    if(beg==node){

        set->first = beg->next;
        set->freeElement(beg->value);
        free(beg);
        (set->size)--;
        return AS_SUCCESS;
    }
    while(beg->next != NULL){
        if(beg->next == node){
            beg->next = node->next;
            set->freeElement(node->value);
            free(node);
            return AS_SUCCESS;
        }
        beg=beg->next;
    }

    return AS_SUCCESS;
}
//CHECK WHY TEST FAILS
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






