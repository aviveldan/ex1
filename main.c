#include "matamazom.h"
//#include "string.h"
#include "assert.h"
#include <stdlib.h>
static MtmProductData copyDouble(MtmProductData number) {
    double *copy = malloc(sizeof(*copy));
    if (copy) {
        *copy = *(double*)number;
    }
    return copy;
}

static void freeDouble(MtmProductData number) {
    free(number);
}

static double simplePrice(MtmProductData basePrice, const double amount) {
    return (*(double*)basePrice) * amount;
}
int main(){
    Matamazom aviv = matamazomCreate();
    double basePrice = 8.9;
    mtmNewProduct(aviv, 4, "Tomato", 2019.11, MATAMAZOM_ANY_AMOUNT, &basePrice, copyDouble, freeDouble, simplePrice);
    unsigned int id = mtmCreateNewOrder(aviv);
    printf("id: %d\n",id);
    int result = mtmChangeProductAmountInOrder(aviv,1,4,3);
    printf("result: %d\n",result);
    matamazomDestroy(aviv);
    return 0;
}
