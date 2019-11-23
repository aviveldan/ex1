//
// Created by avive on 21/11/2019.
//
#include "stdlib.h"
#include "matamazom.h"
#include "set.h"
#include "list.h"
#include "amount_set.h"
typedef enum OrderOrProduct_t {
    PRODUCT = 0,
    ORDER = 1,
} OrderOrProduct;





//Order static functions:
static ListElement copyOrder(ListElement order_t);
static void freeOrder(ListElement order_t);
static int compareOrders(ListElement lhs, ListElement rhs);

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



//Data structures:

typedef struct MatamazomProduct_t {
    unsigned int product_id;
    char* name; //allocated
    MatamazomAmountType amountType;
    MtmProductData customData; //allocated
    MtmCopyData copyData;
    MtmFreeData freeData;
    MtmGetProductPrice prodPrice;
} *MatamazomProduct;



typedef struct MatamazomOrder_t {
    unsigned int orderId;
    AmountSet products;
} *MatamazomOrder;


struct Matamazom_t {
    AmountSet products;
    List orders;
    int size;

};





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




//Order functions:
static ListElement copyOrder(ListElement order_t) {
    MatamazomOrder order = order_t;
    MatamazomOrder copy = malloc(sizeof(*copy));
    if (copy != NULL) {
        copy->orderId = order->orderId;
        copy->products = asCopy(order->products);
        if(copy->products == NULL){
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
/*
unsigned int mtmCreateNewOrder(Matamazom matamazom){
    if (matamazom->orders==NULL){
        return
    }
}
*/


Matamazom matamazomCreate() {
    Matamazom matamazom = malloc(sizeof(*matamazom));
    if(matamazom==NULL) {
        return NULL;
    }
    //Creating a products set:




    //Creating an orders list:
    matamazom->orders = listCreate(copyOrder,freeOrder);
    if(matamazom->orders == NULL){
        free(matamazom); //didn't forget HAHA
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



static bool isValidId(List orders, unsigned int Id, OrderOrProduct id_type,ListElement* output_ptr){
    //assert(id_type = ORDER || id_type == PRODUCT);
    LIST_FOREACH(ListElement,i,orders){
        MatamazomOrder current = i;
        switch (id_type){
            case PRODUCT:
                if(asContains(current->products,&Id)){
                    if(output_ptr!=NULL){
                        *output_ptr = i;
                    }
                    return true;
                }
            case ORDER:
                if(getOrderId(i)==Id){
                    if(output_ptr!=NULL){
                        *output_ptr = i;
                    }
                    return true;
                }
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
        if(order==i){
            listRemoveCurrent(list);
            return MATAMAZOM_SUCCESS;
        }
    }
    return MATAMAZOM_SUCCESS;
}







