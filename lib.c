//
// Created by Andrew Yang on 9/17/21.
//

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "lib.h"

static bool nativeError(Value* args, char* msg, ...) {
    va_list list;
    char error[100];
    va_start(list, msg);
    vsprintf(error, msg, list);
    args[-1] = OBJ_VAL(copyString(error, 100));
    va_end(list);
    return false;
}

static char* getType(Value value) {
    if (IS_BOOL(value)) return "bool";
    else if (IS_NUMBER(value)) return "number";
    else if (IS_NIL(value)) return "nil";
    else if (IS_OBJ(value)) {
        switch (objType(AS_OBJ(value))) {
            case OBJ_NATIVE: return "native function";
            case OBJ_BOUND_METHOD: return "bound method";
            case OBJ_INSTANCE: return "instance";
            case OBJ_FUNCTION: return "function";
            case OBJ_STRING: return "string";
            case OBJ_UPVALUE: return "upvalue";
            case OBJ_CLOSURE: return "closure";
            case OBJ_CLASS: return "class";
        }
    }
    // Unreachable.
    return "unknown";
}

bool clockNative(int argCount, Value* args) {
    args[-1] = NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
    return true;
}

bool sqrtNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           "Argument 1 (input) must be of type 'number', not '%s'.", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(sqrt(AS_NUMBER(args[0])));
    return true;
}

bool powNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           "Argument 1 (base) must be of type 'number', not '%s'.", getType(args[0]));
    }
    if (!IS_NUMBER(args[1])) {
        return nativeError(args,
                           "Argument 2 (power) must be of type 'number', not '%s'.", getType(args[1]));
    }
    args[-1] = NUMBER_VAL(pow(AS_NUMBER(args[0]), AS_NUMBER(args[1])));
    return true;
}

bool minNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           "Argument 1 must be of type 'number', not '%s'.", getType(args[0]));
    }
    if (!IS_NUMBER(args[1])) {
        return nativeError(args,
                           "Argument 2 must be of type 'number', not '%s'.", getType(args[1]));
    }
    double a = AS_NUMBER(args[0]);
    double b = AS_NUMBER(args[1]);
    args[-1] = NUMBER_VAL(a < b ? a : b);
    return true;
}

bool maxNative(int argCount, Value* args) {
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           "Argument 1 must be of type 'number', not '%s'.", getType(args[0]));
    }
    if (!IS_NUMBER(args[1])) {
        return nativeError(args,
                           "Argument 2 must be of type 'number', not '%s'.", getType(args[1]));
    }
    double a = AS_NUMBER(args[0]);
    double b = AS_NUMBER(args[1]);
    args[-1] = NUMBER_VAL(a > b ? a : b);
    return true;
}

bool roundNative(int argCount, Value* args) {
    if (argCount < 1 || argCount > 2) {
        return nativeError(args, "Expected 1 to 2 arguments but got %d.", argCount);
    }
    if (!IS_NUMBER(args[0])) {
        return nativeError(args,
                           "Argument 1 (input) must be of type 'number', not '%s'.", getType(args[0]));
    }
    if (argCount == 2 && !IS_NUMBER(args[1])) {
        return nativeError(args,
                           "Argument 2 (precision) must be of type 'number', not '%s'.", getType(args[1]));
    }
    double shift =  pow(10.0, AS_NUMBER(args[1]));
    args[-1] = NUMBER_VAL(round(AS_NUMBER(args[0]) * shift) / shift);
    return true;
}

bool stonNative(int argCount, Value* args) {
    if (!IS_STRING(args[0])) {
        return nativeError(args,
                           "Argument 1 (input) must be of type 'string', not '%s'.", getType(args[0]));
    }
    args[-1] = NUMBER_VAL(strtod(AS_CSTRING(args[0]), NULL));
    return true;
}
