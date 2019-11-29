//
// Created by avive on 21/11/2019.
//
#include "stdlib.h"
#include "matamazom.h"
#include "list.h"
#include "amount_set.h"
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
static void deleteItemFromOrder(MatamazomOrder order,unsigned int product_id);
static bool isItemInOrder(MatamazomOrder order,unsigned int product_id);
static bool isItemInAnyOrder(Matamazom matamazom,unsigned int product_id);
static ListElement copyOrder(ListElement order_t);
static void freeOrder(ListElement order_t);
static int compareOrders(ListElement lhs, ListElement rhs);

static bool newOrder(Matamazom matamazom, MatamazomOrder* target);
static double evaluatePrice(Matamazom matamazom, unsigned int product_id, const double amount);
static MatamazomResult checkoutItem(Matamazom matamazom, unsigned int product_id,const double amount);
static MatamazomOrder getOrderById(Matamazom matamazom,unsigned int id);


//ProductID static functions:

static ASElement copyProductId(ASElement productid_t);
static void freeProductId(ASElement productid_t);
static int compareProductsIds(ASElement lhs, ASElement rhs);


//String static functions:
static int stringLength (const char *string);
static char* stringCopy(const char* source, char* destination);

//Product static functions:
static ASElement copyProduct(ASElement product_t);
static void freeProduct(ASElement product_t);
static int compareProducts(ASElement lhs, ASElement rhs);
static double getProductAmount(Matamazom matamazom,unsigned int product_id); //already implemented
static MatamazomProduct getProductById(Matamazom matamazom ,const unsigned int productId);
static void printInventoryNoHead(Matamazom matamazom, FILE *output);

//checks if the given amount is consistent with product's amount type. *doesn't check if amount<0*
bool isValidAmount(MatamazomAmountType type, const double amount_t) {
    double amount = absolute(amount_t);
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
    if (*name==END_OF_STRING) {
        return 0;
    }
    if ((*name<=LAST_UPPER && *name>=FIRST_UPPER) || (*name<=LAST_LOWER && *name>=FIRST_LOWER) || ((*name<=NINE)&&(*name>=0))) {
        return 1;
    }
    return 0;
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


MatamazomResult mtmNewProduct(Matamazom matamazom, const unsigned int id, const char *name, const double amount,
                              const MatamazomAmountType amountType, const MtmProductData customData,
                              MtmCopyData copyData, MtmFreeData freeData, MtmGetProductPrice prodPrice) {

    MatamazomProduct newProduct = malloc(sizeof(*newProduct));
    if (newProduct == NULL) {
        return MATAMAZOM_OUT_OF_MEMORY;
    }
    if (matamazom==NULL||name==NULL||customData==NULL||copyData==NULL||freeData==NULL||prodPrice==NULL) {
        free(newProduct);
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if(!(isNameValid(name))) {
        free(newProduct);
        return MATAMAZOM_INVALID_NAME;
    }
    if (!(isValidAmount(amountType, amount)) || amount<0) {
        free(newProduct);
        return MATAMAZOM_INVALID_AMOUNT;
    }
    if(idExists(matamazom, id)) {
        free(newProduct);
        return MATAMAZOM_PRODUCT_ALREADY_EXIST;
    }

    newProduct->product_id = id;
    int nameLength = stringLength(name)+1;
    newProduct->name = malloc(nameLength);
    newProduct->name = stringCopy(name, newProduct->name);
    newProduct->amountType = amountType;
    newProduct->customData = copyData(customData);
    newProduct->copyData = copyData;
    newProduct->freeData = freeData;
    newProduct->prodPrice = prodPrice;
    newProduct->income = 0;

    if (asRegister(matamazom->products, newProduct) == AS_OUT_OF_MEMORY) {
        free(newProduct);
        return MATAMAZOM_OUT_OF_MEMORY;
    }

    asChangeAmount(matamazom->products, newProduct, amount);

    return MATAMAZOM_SUCCESS;

}

static ASElement copyProduct(ASElement product_t) {
    return product_t;
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
    return order_t;
}

static void freeOrder(ListElement order_t) {
    static int num = 0;
    if(num >= 1){
        num+=0;
    }
    compareOrders(order_t,order_t);
    if(order_t==NULL){
        return;
    }
    MatamazomOrder order = order_t;
    asClear(order->products);
    asDestroy(order->products);
    free(order);
    num++;
}

static int compareOrders(ListElement lhs, ListElement rhs) {
    if(lhs==NULL||rhs==NULL){
        return 0;
    }
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
    freeOrder(NULL);
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
                                              const unsigned int productId, const double amount) {

    if(matamazom == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    unsigned int order_id = orderId;
    MatamazomOrder order = getOrderById(matamazom,order_id);
    if(order==NULL) {
        return MATAMAZOM_ORDER_NOT_EXIST;
    }
    if(getProductById(matamazom,productId) == NULL) {
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    MatamazomProduct product = getProductById(matamazom, productId);
    if(!isValidAmount(product->amountType, amount)) {
        return MATAMAZOM_INVALID_AMOUNT;
    }

    unsigned int product_id = productId;
    double current_amount_in_order = 0;
    asGetAmount(order->products,&product_id,&current_amount_in_order);
    if(amount<0 && (current_amount_in_order+amount <=0)) {
        asDelete(order->products,&product_id);
        return MATAMAZOM_SUCCESS;
    }

    unsigned int prodId = productId;
    if(!asContains(order->products,&prodId)){
        asRegister(order->products,&prodId);
    }

    unsigned int product_id_t = productId;
    asChangeAmount(order->products,&product_id_t,amount);


    return MATAMAZOM_SUCCESS;
}

MatamazomResult mtmCancelOrder(Matamazom matamazom, const unsigned int orderId){


    if(matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    MatamazomOrder order = getOrderById(matamazom,orderId);
    if(order==NULL){
        return MATAMAZOM_ORDER_NOT_EXIST;
    }


    listGetFirst(matamazom->orders);
    LIST_FOREACH(ListElement,i,matamazom->orders){
        MatamazomOrder current_order = i;
        if(order->orderId==current_order->orderId){
            listRemoveCurrent(matamazom->orders);
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
    free((unsigned int*)productid_t);
}
//static
int compareProductsIds(ASElement lhs, ASElement rhs) {
    return (int)((*(unsigned int *)lhs) - (*(unsigned int *)rhs));
}


MatamazomResult mtmShipOrder(Matamazom matamazom, const unsigned int orderId){
    if(matamazom==NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    MatamazomOrder order = getOrderById(matamazom,orderId);
    if(order==NULL){
        return MATAMAZOM_ORDER_NOT_EXIST;
    }
    //now order variable contains the ListElement of the shipped order.


    //iterate over the order's products and check if amount is valid(in warehouse)
    AS_FOREACH(ASElement,i,order->products){
        double current_amount = 0;
        unsigned int current_product_id = *(unsigned int*)i;
        //get amount from the product AmountSet
        asGetAmount(order->products,i,&current_amount);
        if(current_amount>getProductAmount(matamazom,current_product_id)){
            return MATAMAZOM_INSUFFICIENT_AMOUNT;
        }

    }
    //iterate over the order's products and both change amount in warehouse and add income
    AS_FOREACH(ASElement,i,order->products){
        double current_amount = 0;

        unsigned int current_product_id = *(unsigned int*)i;
        asGetAmount(order->products,&current_product_id,&current_amount);
        //get amount from the product AmountSet
        checkoutItem(matamazom,current_product_id,current_amount);
    }
    mtmCancelOrder(matamazom,orderId);

    return MATAMAZOM_SUCCESS;
}



static MatamazomResult checkoutItem(Matamazom matamazom, unsigned int product_id,const double amount){
    double income = evaluatePrice(matamazom,product_id,amount);
    MatamazomProduct product = getProductById(matamazom,product_id);
    MatamazomResult result = mtmChangeProductAmount(matamazom,product_id,-amount);
    if (result!=MATAMAZOM_SUCCESS) {
        return result;
    }
    //double income = evaluatePrice(matamazom,product_id,amount);
    matamazom->income += income;
    //MatamazomProduct product = getProductById(matamazom,product_id);
    product->income += income;
    return MATAMAZOM_SUCCESS;
}

static double evaluatePrice(Matamazom matamazom, unsigned int product_id, const double amount){
    MatamazomProduct product = getProductById(matamazom,product_id);
    return product->prodPrice(product->customData,amount);
}



static double getProductAmount(Matamazom matamazom, unsigned int product_id){
    MatamazomProduct product = getProductById(matamazom,product_id);
    double amount = 0;
    asGetAmount(matamazom->products,product,&amount);
    return amount;
}

//product printing functions:

MatamazomResult mtmPrintInventory(Matamazom matamazom, FILE *output) {
    if(matamazom==NULL || output == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    fprintf(output,"Inventory Status:\n");
    printInventoryNoHead(matamazom,output);
    return MATAMAZOM_SUCCESS;
}

MatamazomResult mtmPrintBestSelling(Matamazom matamazom, FILE *output) {
    if (matamazom == NULL || output == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    fprintf(output,"Best Selling Product:\n");
    double highestIncome = 0;
    MatamazomProduct highestProduct = asGetFirst(matamazom->products);
    AS_FOREACH(MatamazomProduct, iterator, matamazom->products) {
        MatamazomProduct product = iterator;
        if (product->income>highestIncome) {
            highestIncome = product->income;
            highestProduct = product;
        }
    }
    if(highestIncome==0){
        fprintf(output,"none\n");
        return MATAMAZOM_SUCCESS;
    }
    mtmPrintIncomeLine(highestProduct->name, highestProduct->product_id, highestIncome, output);
    return MATAMAZOM_SUCCESS;
}

MatamazomResult mtmPrintFiltered(Matamazom matamazom, MtmFilterProduct customFilter, FILE *output) {
    if (matamazom == NULL || output == NULL || customFilter == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    Matamazom filteredMatamazom = matamazomCreate();
    AS_FOREACH(MatamazomProduct, iterator, matamazom->products) {
        MatamazomProduct product = iterator;
        double productAmount = getProductAmount(matamazom, product->product_id);
        if (customFilter(product->product_id, product->name, productAmount, product->customData)) {
            mtmNewProduct(filteredMatamazom, product->product_id, product->name, productAmount, product->amountType,
                          product->customData, product->copyData, product->freeData, product->prodPrice);
        }
    }
    printInventoryNoHead(filteredMatamazom, output);
    matamazomDestroy(filteredMatamazom);
    return MATAMAZOM_SUCCESS;
}

static void printInventoryNoHead(Matamazom matamazom, FILE *output){
    AS_FOREACH(MatamazomProduct, iterator, matamazom->products) {
        MatamazomProduct product = iterator;
        double productAmount = 0;
        asGetAmount(matamazom->products, product, &productAmount);
        double productPrice = product->prodPrice(product->customData, 1);
        mtmPrintProductDetails(product->name, product->product_id, productAmount, productPrice, output);
    }
    return MATAMAZOM_SUCCESS;
}


MatamazomResult mtmPrintOrder(Matamazom matamazom, const unsigned int orderId, FILE *output) {
    MatamazomOrder order = getOrderById(matamazom,orderId);
    if(matamazom == NULL || output == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if(order==NULL) {
        return MATAMAZOM_ORDER_NOT_EXIST;
    }

    mtmPrintOrderHeading(orderId,output);
    //AS of order's products:
    AmountSet products_in_order = order->products;

    double total_price = 0;
    AS_FOREACH(ASElement,i,products_in_order){
        //Current product details:
        unsigned int product_id = *(unsigned int*)i;
        double amount_in_order = 0;
        asGetAmount(products_in_order,i,&amount_in_order);
        double price_for_product = evaluatePrice(matamazom,product_id,amount_in_order);
        MatamazomProduct product = getProductById(matamazom,product_id);
        //Print details to file
        mtmPrintProductDetails(product->name,product_id,amount_in_order,price_for_product,output);
        total_price += price_for_product;
    }
    //Print order summary
    mtmPrintOrderSummary(total_price,output);
    return AS_SUCCESS;
}

MatamazomResult mtmClearProduct(Matamazom matamazom, const unsigned int id){
    if(matamazom==NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }

    if(getProductById(matamazom,id)==NULL){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    unsigned int id_t = id;
    //delete items from orders
    if(isItemInAnyOrder(matamazom,id)) {
        LIST_FOREACH(ListElement, i, matamazom->orders) {
            if (isItemInOrder(((MatamazomOrder) i), id_t)) {
                deleteItemFromOrder(((MatamazomOrder) i), id_t);
            }
        }
    }
    //delete items from warehouse
    MatamazomProduct item = getProductById(matamazom,id);
    asDelete(matamazom->products,item);
    return MATAMAZOM_SUCCESS;
}

static MatamazomOrder getOrderById(Matamazom matamazom,unsigned int id){
    if(matamazom==NULL || id==0){
        return NULL;
    }
    LIST_FOREACH(ListElement,i,matamazom->orders){
        MatamazomOrder order = i;
        if(order->orderId==id){
            return order;
        }
    }
    return NULL;
}

static bool isItemInOrder(MatamazomOrder order,unsigned int product_id){
    if(order == NULL || product_id==0){
        return false;
    }
    return asContains(order->products,&product_id);
}

static bool isItemInAnyOrder(Matamazom matamazom,unsigned int product_id){
    if(matamazom == NULL || product_id == 0){
        return false;
    }
    LIST_FOREACH(ListElement,i,matamazom->orders){
        MatamazomOrder current_order = i;
        if(isItemInOrder(current_order,product_id)){
            return true;
        }
    }
    return false;
}

static void deleteItemFromOrder(MatamazomOrder order,unsigned int product_id){
    if(order==NULL||product_id==0){
        return;
    }
    asDelete(order->products,&product_id);
}