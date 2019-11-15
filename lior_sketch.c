//
// Created by Lior on 15/11/2019.
//
#include "amount_set.h"
#include <stdlib.h>

typedef struct set {
    struct set* next;
    void *value;
} *Set;

int main() {
    Set set1 = malloc(sizeof(*set1));
    Set set2 = malloc(sizeof(*set2));

    set1->value = 1;
    set1->next = set2;
    set2->value = 2;
    set2->next = NULL;
    Set curr=set1;
    Set next = set1->next;
    while(curr){
        curr=curr->next;
    }
}