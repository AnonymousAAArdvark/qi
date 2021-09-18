//
// Created by Andrew Yang on 9/17/21.
//

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "stdlib.h"

static bool clockNative(int argCount, Value* args) {
    args[-1] = NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
    return true;
}

static bool sqrtNative(int argCount, Value* args) {
    args[-1] = NUMBER_VAL(sqrt(AS_NUMBER(args[0])));
    return true;
}

static bool powNative(int argCount, Value* args) {
    args[-1] = NUMBER_VAL(pow(AS_NUMBER(args[0]), AS_NUMBER(args[1])));
    return true;
}

static bool minNative(int argCount, Value* args) {
    double a = AS_NUMBER(args[0]);
    double b = AS_NUMBER(args[1]);
    args[-1] = NUMBER_VAL(a < b ? a : b);
    return true;
}

static bool maxNative(int argCount, Value* args) {
    double a = AS_NUMBER(args[0]);
    double b = AS_NUMBER(args[1]);
    args[-1] = NUMBER_VAL(a > b ? a : b);
    return true;
}

static bool roundNative(int argCount, Value* args) {
    int precision = 0;
    if (argCount < 1 || argCount > 2) {
        char error[40];
        sprintf(error, "Expected 1 to 2 arguments but got %d", argCount);
        args[-1] = OBJ_VAL(copyString(error, 40));
        return false;
    }
    return true;
}