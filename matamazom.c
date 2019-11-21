//
// Created by avive on 21/11/2019.
//
#include "stdlib.h"
#include "matamazom.h"
#include "set.h"
#include "list.h"


typedef struct MatamazomProduct_t {
    char* name;
    unsigned int id;
    MtmProductData data;
    MatamazomAmountType type;
} *MatamazomProduct;

typedef struct MatamazomOrder_t {

} *MatamazomOrder;


struct Matamazom_t{
    MatamazomProduct first_product;
    MatamazomOrder first_order;
    int size;

};

Matamazom matamazomCreate() {
    Matamazom matamazom = malloc(sizeof(*matamazom));
    if
}




