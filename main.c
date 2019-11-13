#include <stdio.h>
#include <stdlib.h>
typedef struct set_element{
    int amount;
    struct set_element* next;
}*SetElement;

int main() {
    SetElement ele1 = malloc(sizeof(*ele1));
    SetElement ele2 = malloc(sizeof(*ele2));
    SetElement ele3 = malloc(sizeof(*ele3));
    SetElement ele4 = malloc(sizeof(*ele4));
    SetElement ele5 = malloc(sizeof(*ele5));
    ele1->next=ele2;
    ele2->next=ele3;
    ele3->next=ele4;
    ele4->next=ele5;
    ele5->next=NULL;
    SetElement temp = ele1;
    int count = 0;
    while(temp){
        temp->amount = ++count;
        temp=temp->next;
    }

    temp = ele1;
    while(temp){
        printf("%d",temp->amount);
        temp=temp->next;
    }
    //printf("%d",ele1->amount);
    return 0;
}