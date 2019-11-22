//
// Created by avive on 21/11/2019.
//
#include "stdlib.h"
#include "matamazom.h"
#include "set.h"
#include "list.h"
#include "amount_set.h"
#include "../../../../cygwin64/usr/include/sys/types.h"


typedef struct MatamazomProduct_t {
    char* name;
    unsigned int id;
    MtmProductData data;
    MatamazomAmountType type;
} *MatamazomProduct;

typedef struct MatamazomOrder_t {
    unsigned int orderId;
    AmountSet products;
} *MatamazomOrder;


struct Matamazom_t{
    AmountSet products;
    List orders;
    int size;
};

static ListElement copyOrder(ListElement order) {
    MatamazomOrder *copy = malloc(sizeof(*copy));
    if (copy != NULL) {
        copy->orderId = order->orderId;
        copy->products = asCopy(order->products);
        if(copy->products = NULL){
            free(copy);
            copy==NULL;
        }
    }
    return copy;
}

static void freeOrder(ListElement order) {
    asDestroy(order->products)
    free(number);
}

static int compareInts(ListElement lhs, ListElement rhs) {
    return (*(int *)lhs) - (*(int *)rhs);
}

unsigned int mtmCreateNewOrder(Matamazom matamazom){
    if (matamazom->orders==NULL){
        listCreate()
    }
}

Matamazom matamazomCreate() {
    Matamazom matamazom = malloc(sizeof(*matamazom));
    if(matamazom==NULL){
        return NULL;
    }
    matamazom->orders = listCreate();
}