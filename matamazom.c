//
// Created by avive on 21/11/2019.
//
#include "stdlib.h"
#include "matamazom.h"
#include "list.h"
#include "amount_set.h"
#include <assert.h>
#include "matamazom_print.h"


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
static bool idExists(Matamazom matamazom, unsigned int id);

//Order static functions:


static ListElement copyOrder(ListElement order_t);
static void freeOrder(ListElement order_t);
static int compareOrders(ListElement lhs, ListElement rhs);

static bool newOrder(Matamazom matamazom, MatamazomOrder* target);
static double totalOrderPrice(Matamazom matamazom, MatamazomOrder orderId);
static double evaluatePrice(Matamazom matamazom, unsigned int product_id, const double amount);
static MatamazomResult checkoutItem(Matamazom matamazom, unsigned int product_id,const double amount);



//ProductID static functions:

static ASElement copyProductId(ASElement productid_t);
static void freeProductId(ASElement productid_t);
static int compareProductsIds(ASElement lhs, ASElement rhs);

/*
ASElement copyProductId(ASElement productid_t);
void freeProductId(ASElement productid_t);
int compareProductsIds(ASElement lhs, ASElement rhs);
 */
/*
Function for validating id of order/product in order.
id_type: For order/product use keyword ORDER/PRODUCT
output_ptr: If you also wants to know the address of the found order - pass an output pointer.
just pass NULL to output_ptr if you do not need.
 */
static bool isValidId(Matamazom matamazom, unsigned int Id, OrderOrProduct id_type,ListElement* output_ptr);
static unsigned int getOrderId(ListElement order_t);

//String static functions:
static int stringLength (const char *string);
static char* stringCopy(const char* source, char* destination);

//Product static functions:
static ASElement copyProduct(ASElement product_t);
static void freeProduct(ASElement product_t);
static int compareProducts(ASElement lhs, ASElement rhs);
static double getProductAmount(Matamazom matamazom,unsigned int product_id); //already implemented
static MatamazomProduct getProductById(Matamazom matamazom ,const unsigned int productId);

//checks if the given amount is consistent with product's amount type. *doesn't check if amount<0*
bool isValidAmount(MatamazomAmountType type, const double amount) {
    /*if (amount<0) {
        return false;
    }*/
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


//implementing strings copy and strings length
static char* stringCopy(const char* source, char* destination) {
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

static int stringLength (const char *string) {
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

//checks if there is a product with the given id
static bool idExists(Matamazom matamazom, unsigned int id) {
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
    if (!(isValidAmount(amountType, amount)) || amount<0) {
        return MATAMAZOM_INVALID_AMOUNT;
    }
    if(idExists(matamazom, id)) {
        return MATAMAZOM_PRODUCT_ALREADY_EXIST;
    }

    newProduct->product_id = id;
    int nameLength = stringLength(name);
    newProduct->name = malloc(sizeof(nameLength));
    newProduct->name = stringCopy(name, newProduct->name);
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
    char *copy_name = malloc(sizeof(stringLength(product->name)));
    if (copy_name==NULL) {
        return NULL;
    }
    copy->name = copy_name;
    stringCopy(product->name,copy->name);
    copy->amountType = product->amountType;
    copy->copyData = product->copyData;
    copy->freeData = product->freeData;
    copy->prodPrice = product->prodPrice;
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

MatamazomResult mtmChangeProductAmount(Matamazom matamazom, const unsigned int id, const double amount) {
    if (matamazom==NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (!idExists(matamazom, id)) {
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    MatamazomProduct product = getProductById(matamazom, id);
    if (!isValidAmount(product->amountType, amount)) {
        return MATAMAZOM_INVALID_AMOUNT;
    }
    double productAmount;
    asGetAmount(matamazom->products, product, &productAmount);
    if (productAmount+amount<0) {
        return MATAMAZOM_INSUFFICIENT_AMOUNT;
    }
    asChangeAmount(matamazom->products, product, amount);
    return MATAMAZOM_SUCCESS;
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
    if(order_t==NULL){
        return;
    }
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
    matamazom->currentId = 0;
    matamazom->income = 0;
    matamazom->products = NULL;
    matamazom->orders = NULL;

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

static MatamazomProduct getProductById(Matamazom matamazom ,const unsigned int productId) {
    AS_FOREACH(ASElement,i,matamazom->products) {
        MatamazomProduct currentProduct = i;
        if(currentProduct->product_id == productId) {
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
    if(!isValidId(matamazom,orderId,ORDER,&searched_ptr)){
        return MATAMAZOM_ORDER_NOT_EXIST;
    }
    /*
    if(!isValidId(matamazom,productId,PRODUCT,NULL)){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
     */
    MatamazomProduct product = getProductById(matamazom, productId);
    if(!isValidAmount(product->amountType, amount)){
        return MATAMAZOM_INVALID_AMOUNT;
    }
    MatamazomOrder actual_order = searched_ptr;
    unsigned int prodId = productId;
    if(!asContains(actual_order->products,&prodId)){
        asRegister(actual_order->products,&prodId);
    }
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
}



static bool isValidId(Matamazom matamazom, unsigned int id, OrderOrProduct id_type,ListElement* output_ptr){
    assert(id_type == ORDER || id_type == PRODUCT);
    LIST_FOREACH(ListElement,i,matamazom->orders){
        MatamazomOrder current = i;
        switch (id_type){
            case PRODUCT:
                if(true){
                    //unsigned int first =*((unsigned int*)asGetFirst(current->products));
                    //MatamazomProduct product = getProductById(matamazom,id);
                    if(asContains(current->products,&id)) {
                        if (output_ptr != NULL) {
                            *output_ptr = i;
                        }
                        return true;
                    }
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
    if(!isValidId(matamazom,orderId,ORDER,&order)){
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



//static
static ASElement copyProductId(ASElement productid_t) {
    unsigned int *copy = malloc(sizeof(*copy));
    if (copy != NULL) {
        *copy = *(unsigned int *)productid_t;
    }
    return copy;
}
//void
static void freeProductId(ASElement productid_t) {
    free(productid_t);
}
//static
int compareProductsIds(ASElement lhs, ASElement rhs) {
    return (int)((*(unsigned int *)lhs) - (*(unsigned int *)rhs));
}


MatamazomResult mtmShipOrder(Matamazom matamazom, const unsigned int orderId){
    if(matamazom==NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    ListElement order = NULL;
    if(!isValidId(matamazom,orderId,ORDER,&order)){
        return MATAMAZOM_ORDER_NOT_EXIST;
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


//waiting for lior's mtmChangeProductAmount
static MatamazomResult checkoutItem(Matamazom matamazom, unsigned int product_id,const double amount){
    MatamazomResult result = 0;//mtmChangeProductAmount(matamazom,product_id,amount);
    if (result!=MATAMAZOM_SUCCESS) { //***aviv needs to fix it
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


static double getProductAmount(Matamazom matamazom, unsigned int product_id){
    MatamazomProduct product = getProductById(matamazom,product_id);
    double amount = 0;
    int getAmountResult = asGetAmount(matamazom->products,product,&amount);
    assert(getAmountResult == AS_SUCCESS);
    return amount;
}

MatamazomResult mtmPrintInventory(Matamazom matamazom, FILE *output) {

}


MatamazomResult mtmPrintOrder(Matamazom matamazom, const unsigned int orderId, FILE *output) {
    ListElement order = NULL;
    if(matamazom == NULL || output == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if(!isValidId(matamazom,orderId,ORDER,&order)){
        return MATAMAZOM_ORDER_NOT_EXIST;
    }

    mtmPrintOrderHeading(orderId,output);
    //AS of order's products:
    AmountSet products_in_order = ((MatamazomOrder)order)->products;

    double total_price = 0;
    AS_FOREACH(ASElement,i,products_in_order){
        //Current product details:
        unsigned int product_id = *(unsigned int*)i;
        double amount_in_order = 0;
        int asResult =  asGetAmount(products_in_order,i,&amount_in_order);
        double price_for_product = evaluatePrice(matamazom,product_id,amount_in_order);
        assert(asResult==AS_SUCCESS);
        MatamazomProduct product = getProductById(matamazom,product_id);
        //Print details to file
        mtmPrintProductDetails(product->name,product_id,amount_in_order,price_for_product,output);
        total_price += price_for_product;
    }
    //Print order summary
    mtmPrintOrderSummary(total_price,output);
}

MatamazomResult mtmClearProduct(Matamazom matamazom, const unsigned int id){
    if(matamazom==NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if(!isValidId(matamazom,id,PRODUCT,NULL)){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    unsigned int id_t = id;
    int asResult = 0;
    //delete items from orders
    LIST_FOREACH(ListElement,i,matamazom->orders){
        AmountSet products_in_order = ((MatamazomOrder)i)->products;
        if(asContains(products_in_order,&id_t)){
            asResult = asDelete(products_in_order,&id_t);
            assert(asResult == AS_SUCCESS);
        }
    }
    //delete items from warehouse
    MatamazomProduct item = getProductById(matamazom,id);
    asResult = asDelete(matamazom->products,item);
    assert(asResult == AS_SUCCESS);
    return MATAMAZOM_SUCCESS;
}