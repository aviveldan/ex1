//
// Created by avive on 21/11/2019.
//
#include "stdlib.h"
#include "matamazom.h"
#include "set.h"
#include "list.h"
#include "amount_set.h"

typedef struct MatamazomProduct_t {
    unsigned int product_id;
    char* name; //allocated
    MatamazomAmountType amountType;
    MtmProductData customData; //allocated
    MtmCopyData copyData;
    MtmFreeData freeData;
    MtmGetProductPrice prodPrice;
} *MatamazomProduct;

//implementing strings copy and strings length
char* string_copy(const char* source, char* destination) {
    if (destination == NULL) {
        return NULL;
    }
    char *ptr = destination;
    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return ptr;
}

int string_length (const char *string) {
    int i=0;
    while (string[i] != '\0') {
        i++;
    }
    return i;
}


MatamazomResult mtmNewProduct(Matamazom matamazom, const unsigned int id, const char *name,
                              const double amount, const MatamazomAmountType amountType,
                              const MtmProductData customData, MtmCopyData copyData,
                              MtmFreeData freeData, MtmGetProductPrice prodPrice) {


}


static ASElement copyProduct(ASElement product_t) {
    MatamazomProduct product = product_t;
    MatamazomProduct copy = malloc(sizeof(*copy));
    copy->product_id = product->product_id;
    char *copy_name = malloc(sizeof(string_length(product->name)));
    string_copy(copy_name, copy->name);
    copy->amountType = product->amountType;
    copy->customData = product->copyData(product->customData);
    return copy;
}

static void freeProduct(ASElement product_t) {
    MatamazomProduct product = product_t;
    free(product->name);
    product->freeData(product->customData);
    free(product);
}

static int compareProducts(ASElement lhs, ASElement rhs) {
    MatamazomProduct leftProduct = lhs;
    MatamazomProduct rightProduct = rhs;
    unsigned int leftId = leftProduct->product_id;
    unsigned int rightId = rightProduct->product_id;
    return ((int)(leftId-rightId));
}


typedef struct MatamazomOrder_t {
    unsigned int orderId;
    AmountSet products;
} *MatamazomOrder;


struct Matamazom_t {
    AmountSet products;
    List orders;
    int size;

};

static ListElement copyOrder(ListElement order) {
    MatamazomOrder *copy = malloc(sizeof(*copy));
    if (copy != NULL) {
        copy->orderId = order->orderId;
        copy->products = asCopy(order->products);
        if(copy->products = NULL) {
            free(copy);
            copy=NULL;
        }
    }
    return copy;
}

static void freeOrder(ListElement order) {
    asDestroy(order->products);
    free(order);
}

static int compareOrders(ListElement lhs, ListElement rhs) {
    return (*(int *)lhs->orderId) - (*(int *)rhs->orderId);
}

unsigned int mtmCreateNewOrder(Matamazom matamazom){
    if (matamazom->orders==NULL) {
        listCreate()
    }
}

Matamazom matamazomCreate() {
    Matamazom matamazom = malloc(sizeof(*matamazom));
    if(matamazom==NULL) {
        return NULL;
    }
    matamazom->orders = listCreate();
}