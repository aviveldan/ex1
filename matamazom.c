//
// Created by avive on 21/11/2019.
//
#include "stdlib.h"
#include "matamazom.h"
#include "list.h"
#include "amount_set.h"
#include <assert.h>

#define END_OF_STRING '\0'
#define FIRST_LOWER 'a'
#define LAST_LOWER 'z'
#define FIRST_UPPER 'A'
#define LAST_UPPER 'Z'
#define NINE 9
#define LEGAL_DIFFERENCE 0.001
#define HALF 0.5

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

static double absolute(double number);
static bool IdExists(Matamazom matamazom, unsigned int id);

//Order static functions:
static ListElement copyOrder(ListElement order_t);
static void freeOrder(ListElement order_t);
static int compareOrders(ListElement lhs, ListElement rhs);
static bool newOrder(Matamazom matamazom, MatamazomOrder* target);
static double totalOrderPrice(Matamazom matamazom, MatamazomOrder orderId);

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
static double getProductAmount(Matamazom matamazom,unsigned int product_id);

bool isValidAmount(MatamazomAmountType type, const double amount) {
    if (amount<0) {
        return false;
    }
    if (type==MATAMAZOM_ANY_AMOUNT) {
        return true;
    }
    int floor = (int)(amount);
    double difference = amount-floor;
    if (type==MATAMAZOM_INTEGER_AMOUNT) {
        if (difference<=LEGAL_DIFFERENCE || (1-difference)<LEGAL_DIFFERENCE) {
            return true;
        }
    }
    if(type==MATAMAZOM_HALF_INTEGER_AMOUNT) {
        if(difference<=LEGAL_DIFFERENCE || (1-difference)<LEGAL_DIFFERENCE||
            absolute(difference-HALF)<LEGAL_DIFFERENCE) {
            return true;
        }
    }
    return false;
}

static double absolute(double number) {
    if(number<0) {
        return (-number);
    } else {
        return number;
    }
}
//aviv needs:
static MatamazomResult checkoutItem(Matamazom matamazom, unsigned int product_id,const double amount);
static double evaluatePrice(Matamazom matamazom, unsigned int product_id, const double amount);


//implementing strings copy and strings length
static char* string_copy(const char* source, char* destination) {
    if (destination == NULL) {
        return NULL;
    }
    char *ptr = destination;
    while (*source != END_OF_STRING) {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = END_OF_STRING;
    return ptr;
}

static int string_length (const char *string) {
    int i=0;
    while (string[i] != END_OF_STRING) {
        i++;
    }
    return i;
}

static bool isNameValid(const char *name) {
    int i=0;
    if (*name!=END_OF_STRING||((*name<=LAST_UPPER)&&(*name>=FIRST_UPPER))||((*name<=LAST_LOWER)&&(*name>=FIRST_LOWER))
        ||((*name<=NINE)&&(*name>=0))) {
        return 1;
    } else {
        return 0;
    }
}

static bool IdExists(Matamazom matamazom, unsigned int id) {
    AS_FOREACH(ASElement, iterator, matamazom->products) {
        MatamazomProduct current = iterator;
        if(current->product_id==id) {
            return true;
        }
    }
    return false;
}


MatamazomResult mtmNewProduct(Matamazom matamazom, const unsigned int id, const char *name,
                              const double amount, const MatamazomAmountType amountType,
                              const MtmProductData customData, MtmCopyData copyData,
                              MtmFreeData freeData, MtmGetProductPrice prodPrice) {

    MatamazomProduct newProduct = malloc(sizeof(*newProduct));
    if (newProduct == NULL) {
        return MATAMAZOM_OUT_OF_MEMORY;
    }
    if (matamazom==NULL||name==NULL||customData==NULL||copyData==NULL||freeData==NULL||prodPrice==NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if(!(isNameValid(name))) {
        return MATAMAZOM_INVALID_NAME;
    }
    if (!(isValidAmount(amountType, amount))) {
        return MATAMAZOM_INVALID_AMOUNT;
    }
    if(IdExists(matamazom, id)) {
        return MATAMAZOM_PRODUCT_ALREADY_EXIST;
    }

    newProduct->product_id = id;
    string_copy(name, newProduct->name);
    newProduct->amountType = amountType;
    newProduct->customData = copyData(customData);
    newProduct->copyData = copyData;
    newProduct->freeData = freeData;
    newProduct->prodPrice = prodPrice;


    if (asRegister(matamazom->products, newProduct) == AS_OUT_OF_MEMORY) {
        return MATAMAZOM_OUT_OF_MEMORY;
    }
    asChangeAmount(matamazom->products, newProduct, amount);

    return MATAMAZOM_SUCCESS;

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


void matamazomDestroy(Matamazom matamazom){
    if(matamazom==NULL){
        return;
    }
    listDestroy(matamazom->orders);
    asDestroy(matamazom->products);
    free(matamazom);
}

static MatamazomProduct getProductById(Matamazom matamazom ,const unsigned int productId){
    AS_FOREACH(ASElement,i,matamazom->products){
        MatamazomProduct currentProduct = i;
        if(currentProduct->product_id == productId){
            return currentProduct;
        }
    }
    return NULL;
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
    if(!isValidAmount(getProductById(matamazom, productId), amount)){
        return MATAMAZOM_INVALID_AMOUNT;
    }
    MatamazomOrder actual_order = searched_ptr;

    int asChangeAmountReturns = asChangeAmount(actual_order->products,(ASElement)&productId,amount);

    assert(asChangeAmountReturns == AS_SUCCESS); //needs to learn how to use assert

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
    assert(id_type == ORDER || id_type == PRODUCT);
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


MatamazomResult mtmShipOrder(Matamazom matamazom, const unsigned int orderId){
    if(matamazom==NULL){
        MATAMAZOM_NULL_ARGUMENT;
    }
    ListElement order = NULL;
    if(!isValidId(matamazom->orders,orderId,ORDER,&order)){
        MATAMAZOM_ORDER_NOT_EXIST;
    }
    //now order variable contains the ListElement of the shipped order.
    MatamazomOrder actual_order = order;

    //iterate over the order's products and check if amount is valid(in warehouse)
    AS_FOREACH(ASElement,i,actual_order->products){
        double current_amount = 0;
        unsigned int current_product_id = *(unsigned int*)i;
        //get amount from the product AmountSet
        assert(asGetAmount(actual_order->products,i,&current_amount) == AS_SUCCESS);
        if(current_amount>getProductAmount(matamazom,current_product_id)){
            return MATAMAZOM_INSUFFICIENT_AMOUNT;
        }

    }
    //iterate over the order's products and both change amount in warehouse and add income
    AS_FOREACH(ASElement,i,actual_order->products){
        double current_amount = 0;
        unsigned int current_product_id = *(unsigned int*)i;
        //get amount from the product AmountSet
        checkoutItem(matamazom,current_product_id,current_amount);
    }
    mtmCancelOrder(matamazom,orderId);
    return MATAMAZOM_SUCCESS;
}

static MatamazomResult checkoutItem(Matamazom matamazom, unsigned int product_id,const double amount){
    MatamazomResult result = mtmChangeProductAmount(matamazom,product_id,amount);
    if (result!=MATAMAZOM_SUCCESS){
        return result;
    }
    double income = evaluatePrice(matamazom,product_id,amount);
    matamazom->income += income;
    MatamazomProduct product = getProductById(matamazom,product_id);
    product->income += income;
    return MATAMAZOM_SUCCESS;
}

static double evaluatePrice(Matamazom matamazom, unsigned int product_id, const double amount){
    double price = 0;
    AmountSet products = matamazom->products;
    AS_FOREACH(ASElement, i, products){
        MatamazomProduct current_product = i;
        if(current_product->product_id == product_id){
            price = current_product->prodPrice(current_product->customData,amount);
        }
    }
    return price;
}

static double totalOrderPrice(Matamazom matamazom, MatamazomOrder orderId){
    double sum = 0;
    AmountSet products = orderId->products;
    AS_FOREACH(ASElement,i,products){
        unsigned int current_product = *(int*)i;
        double current_product_amount = 0;
        assert(asGetAmount(products,i,&current_product_amount)==AS_SUCCESS);
        sum+=(evaluatePrice(matamazom,current_product,current_product_amount));
    }
    return sum;
}
