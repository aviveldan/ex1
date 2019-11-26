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
    basePrice = 3;
    mtmNewProduct(aviv, 3, "Cucumber", 1500.55, MATAMAZOM_ANY_AMOUNT, &basePrice, copyDouble, freeDouble, simplePrice);
    unsigned int id = mtmCreateNewOrder(aviv);
    printf("id: %d\n",id);
    int result = mtmChangeProductAmountInOrder(aviv,1,4,3);
    result = mtmChangeProductAmountInOrder(aviv,1,3,3);
    printf("result: %d\n",result);
    FILE* my_file = fopen("my_file.txt","w");
    mtmPrintOrder(aviv,1,my_file);
    fclose(my_file);

    mtmClearProduct(aviv,4);
    FILE* my_file2 = fopen("my_file2.txt","w");
    mtmPrintOrder(aviv,1,my_file);
    fclose(my_file);


    matamazomDestroy(aviv);

    return 0;
}
