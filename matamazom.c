//
// Created by avive on 21/11/2019.
//
#include "stdlib.h"
#include "matamazom.h"
#include "list.h"
#include "amount_set.h"


typedef enum OrderOrProduct_t {
    PRODUCT = 0,
    ORDER = 1,
} OrderOrProduct;



//Data structures:

typedef struct MatamazomProduct_t {
    unsigned int product_id;
    char* name; //allocated
    MatamazomAmountType amountType;
    MtmProductData customData; //allocated
    MtmCopyData copyData;
    MtmFreeData freeData;
    MtmGetProductPrice prodPrice;
    double income;
} *MatamazomProduct;

typedef struct MatamazomOrder_t {
    unsigned int orderId;
    AmountSet products; //allocated
} *MatamazomOrder;

struct Matamazom_t {
    AmountSet products; //allocated
    List orders; //allocated
    unsigned int currentId;
    double income;
};






//Order static functions:
static ListElement copyOrder(ListElement order_t);
static void freeOrder(ListElement order_t);
static int compareOrders(ListElement lhs, ListElement rhs);
static bool newOrder(Matamazom matamazom, MatamazomOrder* target);

//ProductID static functions:
static ASElement copyProductId(ASElement productid_t);
static void freeProductId(ASElement productid_t);
static int compareProductsIds(ASElement lhs, ASElement rhs);
/*
Function for validating id of order/product in order.
id_type: For order/product use keyword ORDER/PRODUCT
output_ptr: If you also wants to know the address of the found order - pass an output pointer.
just pass NULL to output_ptr if you do not need.
 */
static bool isValidId(List orders, unsigned int Id, OrderOrProduct id_type,ListElement* output_ptr);
static unsigned int getOrderId(ListElement order_t);

//String static functions:
static int string_length (const char *string);
static char* string_copy(const char* source, char* destination);

//Product static functions:
static ASElement copyProduct(ASElement product_t);
static void freeProduct(ASElement product_t);
static int compareProducts(ASElement lhs, ASElement rhs);

//lior needs to implement this function:
bool isValidAmount(Matamazom matamazom,unsigned int productId,const double amount);





//implementing strings copy and strings length
static char* string_copy(const char* source, char* destination) {
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

static int string_length (const char *string) {
    int i=0;
    while (string[i] != '\0') {
        i++;
    }
    return i;
}

static MatamazomProduct createNewProduct() {

}

MatamazomResult mtmNewProduct(Matamazom matamazom, const unsigned int id, const char *name,
                              const double amount, const MatamazomAmountType amountType,
                              const MtmProductData customData, MtmCopyData copyData,
                              MtmFreeData freeData, MtmGetProductPrice prodPrice) {

    MatamazomProduct product = malloc();

    asRegister(matamazom->products, product);
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




//Order functions:


static ListElement copyOrder(ListElement order_t) {
    MatamazomOrder order = order_t;
    MatamazomOrder copy = malloc(sizeof(*copy));
    if (copy != NULL) {
        copy->orderId = order->orderId;
        copy->products = asCopy(order->products);
        if(copy->products == NULL) {
            free(copy);
            copy=NULL;
        }
    }
    return copy;
}

static void freeOrder(ListElement order_t) {
    MatamazomOrder order = order_t;
    asDestroy(order->products);
    free(order);
}

static int compareOrders(ListElement lhs, ListElement rhs) {
    MatamazomOrder left = lhs;
    MatamazomOrder right = rhs;
    return (int)((left->orderId)-(right->orderId));
}

static bool newOrder(Matamazom matamazom, MatamazomOrder* target){
    MatamazomOrder temp = malloc(sizeof(*temp));
    if(temp == NULL){
        return false;
    }
    temp->products = asCreate(copyProductId,freeProductId,compareProductsIds);
    if(temp->products == NULL){
        free(temp);
        return false;
    }
    temp->orderId = ++(matamazom->currentId);
    *target = temp;
    return true;
}

unsigned int mtmCreateNewOrder(Matamazom matamazom){
    if(matamazom == NULL){
        return 0;
    }
    MatamazomOrder new_order = NULL;
    newOrder(matamazom,&new_order);
    if(new_order == false){
        return 0;
    }
    listInsertLast(matamazom->orders,(ListElement)new_order);
    return new_order->orderId;
}



Matamazom matamazomCreate() {
    Matamazom matamazom = malloc(sizeof(*matamazom));
    if(matamazom==NULL) {
        return NULL;
    }
    //Creating a products set:
    matamazom->products = asCreate(copyProduct, freeProduct, compareProducts);
    if (matamazom->products == NULL) {
            free(matamazom);
            return NULL;
    }

    //Creating an orders list:
    matamazom->orders = listCreate(copyOrder,freeOrder);
    if(matamazom->orders == NULL){
        free(matamazom); //didn't forget HAHA
        asDestroy(matamazom->products);
        return NULL;
    }
    return matamazom;
}

MatamazomResult mtmChangeProductAmountInOrder(Matamazom matamazom, const unsigned int orderId,
                                              const unsigned int productId, const double amount){
    ListElement searched_ptr = NULL;
    if(matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if(!isValidId(matamazom->orders,orderId,ORDER,&searched_ptr)){
        return MATAMAZOM_ORDER_NOT_EXIST;
    }
    if(!isValidId(matamazom->orders,productId,PRODUCT,&searched_ptr)){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    if(!isValidAmount (matamazom, productId, amount)){
        return MATAMAZOM_INVALID_AMOUNT;
    }
    MatamazomOrder actual_order = searched_ptr;

    int asChangeAmountReturns = asChangeAmount(actual_order->products,(ASElement)&productId,amount);

    //assert(asChangeAmountReturns == AS_SUCCESS); //needs to learn how to use assert

    return MATAMAZOM_SUCCESS;
}

static unsigned int getOrderId(ListElement order_t){
    MatamazomOrder order = order_t;
    if(order_t == NULL){
        return 0;
    }
    return (order->orderId);
};



static bool isValidId(List orders, unsigned int id, OrderOrProduct id_type,ListElement* output_ptr){
    //assert(id_type = ORDER || id_type == PRODUCT);
    LIST_FOREACH(ListElement,i,orders){
        MatamazomOrder current = i;
        switch (id_type){
            case PRODUCT:
                if(asContains(current->products,&id)){
                    if(output_ptr!=NULL){
                        *output_ptr = i;
                    }
                    return true;
                }
            case ORDER:
                if(getOrderId(i)==id){
                    if(output_ptr!=NULL){
                        *output_ptr = i;
                    }
                    return true;
                }
            default:
                return false;
        }
    }
    return false;
}

MatamazomResult mtmCancelOrder(Matamazom matamazom, const unsigned int orderId){
    ListElement order = NULL;

    if(matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    List list = matamazom->orders;
    if(!isValidId(list,orderId,ORDER,&order)){
        return MATAMAZOM_ORDER_NOT_EXIST;
    }

    LIST_FOREACH(ListElement,i,matamazom->orders){
        if(compareOrders(order,i)){
            listRemoveCurrent(list);
            return MATAMAZOM_SUCCESS;
        }
    }
    return MATAMAZOM_SUCCESS;
}




static ASElement copyProductId(ASElement productid_t) {
    unsigned int *copy = malloc(sizeof(*copy));
    if (copy != NULL) {
        *copy = *(unsigned int *)productid_t;
    }
    return copy;
}
static void freeProductId(ASElement productid_t) {
    free(productid_t); }

static int compareProductsIds(ASElement lhs, ASElement rhs) {
    return (int)((*(unsigned int *)lhs) - (*(unsigned int *)rhs));
}




