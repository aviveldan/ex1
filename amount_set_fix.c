#define NULL_POINTER_SENT -1
#include "amount_set.h"
#include <stdlib.h>
#define isNULL(arg1,arg2,arg3) \
if((arg1)==NULL||(arg2)==NULL||(arg3)==NULL){return NULL} else void;

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
};//
// Created by avive on 17/11/2019.
//


AmountSet asCreate(CopyASElement copyElement,
                   FreeASElement freeElement,
                   CompareASElements compareElements){
    if(copyElement==NULL || freeElement == NULL || compareElements == NULL){
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
    result->iterator = NULL;
    result->size = 0;

    return result;
}

static void copyNodeData(ASListNode target, ASListNode source, AmountSet set){
    if(target==NULL || source==NULL || set==NULL){
        return;
    }
    target->amount=source->amount;
    target->value = set->copyElement(source->value);
}

bool freeNode(ASListNode node,AmountSet set){
    if(set==NULL || node == NULL){
        return false;
    }
    set->freeElement(node->value);
    free(node);
    return true;
}
void updateNodeValues(AmountSet set, ASListNode node, ASElement value,double amount){
    node->amount=amount;
    node->value = set->copyElement(value);
}
AmountSetResult createNode(AmountSet set, ASElement value,double amount,ASListNode next ,ASListNode before){
    ASListNode newNode = malloc(sizeof(ASListNode));
    if(newNode==NULL){
        return AS_OUT_OF_MEMORY;
    }
    if(set->first == next){ //wants to insert in beginning
        set->first = newNode;
        updateNodeValues(set,newNode,value,amount);
        return AS_SUCCESS;
    }
    else if(next==NULL){ //wants to insert in the end
        before->next = newNode;
        newNode->next=NULL;
        updateNodeValues(set,newNode,value,amount);
        return AS_SUCCESS;
    }
    newNode->next = before->next;
    before->next = newNode;
    updateNodeValues(set,newNode,value,amount);
    return AS_SUCCESS;
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